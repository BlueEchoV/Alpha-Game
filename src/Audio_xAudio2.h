#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

// Forward declarations so callers don't need to include XAudio2.
struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;

struct AudioEngine
{
	// Initialize XAudio2 and the mastering voice (main thread). Returns false on failure.
	bool init();

	// Shut down: release the mastering voice and XAudio2 engine. Safe to call if init() failed or was never called.
	void deinit();

	IXAudio2* pXAudio2 = nullptr;
	IXAudio2MasteringVoice* pMasterVoice = nullptr;
	bool comInitialized = false;
};

struct Sound {
	std::vector<uint8_t> data;
	uint32_t format_tag;
	uint32_t channels;
	uint32_t samples_per_second;
	uint32_t avg_bytes_per_second;
	uint32_t block_align;
	uint32_t bits_per_sample;
	std::vector<IXAudio2SourceVoice*> voice_pool;
};

extern std::unordered_map<std::string, Sound> sounds;

void load_sound(const char* name, const char* file_path);
void load_sounds();
Sound* get_sound(std::string_view name);
void play_sound(std::string_view name);
