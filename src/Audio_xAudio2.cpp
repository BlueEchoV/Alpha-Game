#include "Audio_xAudio2.h"
#include "Weapon.h"

#include <windows.h>
#include <xaudio2.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static AudioEngine* g_engine = nullptr;

std::unordered_map<std::string, Sound> sounds;

bool AudioEngine::init()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
		return false;
	}
	// Only we call CoUninitialize if we were the ones who initialized COM (S_OK). S_FALSE means already inited by someone else.
	this->comInitialized = (hr == S_OK);

	hr = XAudio2Create(&this->pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		if (this->comInitialized) {
			CoUninitialize();
			this->comInitialized = false;
		}
		return false;
	}

	hr = this->pXAudio2->CreateMasteringVoice(&this->pMasterVoice);
	if (FAILED(hr)) {
		this->pXAudio2->Release();
		this->pXAudio2 = nullptr;
		if (this->comInitialized) {
			CoUninitialize();
			this->comInitialized = false;
		}
		return false;
	}

	g_engine = this;
	return true;
}

void AudioEngine::deinit()
{
	if (g_engine == this) {
		g_engine = nullptr;
	}
	if (this->pMasterVoice) {
		this->pMasterVoice->DestroyVoice();
		this->pMasterVoice = nullptr;
	}
	if (this->pXAudio2) {
		this->pXAudio2->Release();
		this->pXAudio2 = nullptr;
	}
	if (this->comInitialized) {
		CoUninitialize();
		this->comInitialized = false;
	}
}

static uint32_t read_u32_le(const uint8_t* p) {
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint16_t read_u16_le(const uint8_t* p) {
	return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

void load_sound(const char* name, const char* file_path) {
	FILE* f = nullptr;
	fopen_s(&f, file_path, "rb");
	if (!f) {
		assert(false);
		return;
	}

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::vector<uint8_t> file_bytes(file_size);
	size_t read_count = fread(file_bytes.data(), 1, file_size, f);
	fclose(f);
	if ((long)read_count != file_size) {
		assert(false);
		return;
	}

	if (file_size < 12 || memcmp(file_bytes.data(), "RIFF", 4) != 0 || memcmp(file_bytes.data() + 8, "WAVE", 4) != 0) {
		assert(false);
		return;
	}

	Sound sound = {};
	bool fmt_found = false;
	bool data_found = false;

	size_t cursor = 12;
	while (cursor + 8 <= (size_t)file_size) {
		const uint8_t* chunk_id = file_bytes.data() + cursor;
		uint32_t chunk_size = read_u32_le(file_bytes.data() + cursor + 4);
		size_t chunk_body = cursor + 8;
		if (chunk_body + chunk_size > (size_t)file_size) {
			break;
		}

		if (memcmp(chunk_id, "fmt ", 4) == 0 && chunk_size >= 16) {
			const uint8_t* p = file_bytes.data() + chunk_body;
			sound.format_tag           = read_u16_le(p + 0);
			sound.channels             = read_u16_le(p + 2);
			sound.samples_per_second   = read_u32_le(p + 4);
			sound.avg_bytes_per_second = read_u32_le(p + 8);
			sound.block_align          = read_u16_le(p + 12);
			sound.bits_per_sample      = read_u16_le(p + 14);
			fmt_found = true;
		}
		else if (memcmp(chunk_id, "data", 4) == 0) {
			sound.data.assign(file_bytes.data() + chunk_body, file_bytes.data() + chunk_body + chunk_size);
			data_found = true;
		}

		// Chunks are padded to even sizes.
		cursor = chunk_body + chunk_size + (chunk_size & 1);
	}

	if (!fmt_found || !data_found) {
		assert(false);
		return;
	}

	sounds[name] = std::move(sound);
}

// Sound names map to assets\audio\<name>.wav by convention. Skips empties and
// names already loaded so weapons can share the same wav without re-parsing.
static void load_sound_by_name(const std::string& name) {
	if (name.empty()) {
		return;
	}
	if (sounds.find(name) != sounds.end()) {
		return;
	}
	std::string path = "assets\\audio\\" + name + ".wav";
	load_sound(name.c_str(), path.c_str());
}

void load_sounds() {
	for (const auto& entry : weapon_data_map) {
		load_sound_by_name(entry.second.fire_sound_name);
		load_sound_by_name(entry.second.impact_sound_name);
	}
}

Sound* get_sound(std::string_view name) {
	auto it = sounds.find(std::string(name));
	if (it != sounds.end()) {
		return &it->second;
	}
	return nullptr;
}

void play_sound(std::string_view name) {
	if (!g_engine || !g_engine->pXAudio2) {
		return;
	}
	Sound* sound = get_sound(name);
	if (!sound || sound->data.empty()) {
		return;
	}

	// Reuse an idle voice from the pool if one exists; otherwise create a new one.
	// Voices stay around for the program's lifetime; XAudio2 tears them down with the engine.
	IXAudio2SourceVoice* voice = nullptr;
	for (IXAudio2SourceVoice* v : sound->voice_pool) {
		XAUDIO2_VOICE_STATE state;
		v->GetState(&state);
		if (state.BuffersQueued == 0) {
			voice = v;
			break;
		}
	}

	if (!voice) {
		WAVEFORMATEX wfx = {};
		wfx.wFormatTag      = (WORD)sound->format_tag;
		wfx.nChannels       = (WORD)sound->channels;
		wfx.nSamplesPerSec  = sound->samples_per_second;
		wfx.nAvgBytesPerSec = sound->avg_bytes_per_second;
		wfx.nBlockAlign     = (WORD)sound->block_align;
		wfx.wBitsPerSample  = (WORD)sound->bits_per_sample;
		wfx.cbSize          = 0;

		HRESULT hr = g_engine->pXAudio2->CreateSourceVoice(&voice, &wfx);
		if (FAILED(hr) || !voice) {
			return;
		}
		sound->voice_pool.push_back(voice);
	}

	XAUDIO2_BUFFER buf = {};
	buf.AudioBytes = (UINT32)sound->data.size();
	buf.pAudioData = sound->data.data();
	buf.Flags      = XAUDIO2_END_OF_STREAM;

	if (FAILED(voice->SubmitSourceBuffer(&buf))) {
		return;
	}
	voice->Start(0);
}
