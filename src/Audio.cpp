#include "Audio.h"

#include <windows.h>
#include <stdexcept>
#include <xaudio2.h>
#include <array>
#include <stdio.h>

// *****Constant Literals*****
// 16 bits per sample.
constexpr uint16_t BITS_PER_SAMPLE = 16;
// 44,100 samples per second.
constexpr uint32_t SAMPLES_PER_SEC = 44100;
// 220 cycles per second (frequency of the audible tone).
constexpr double CYCLES_PER_SEC = 220.0;
// 50% volume.
constexpr double VOLUME = 0.5;
// 10 cycles per audio buffer.
constexpr uint16_t AUDIO_BUFFER_SIZE_IN_CYCLES = 10;
constexpr double PI = 3.14159265358979323846;

// *****Calculated Constants*****
// 200 samples per cycle.
constexpr uint16_t SAMPLES_PER_CYCLE = (uint16_t)(SAMPLES_PER_SEC / CYCLES_PER_SEC);
// 2,000 samples per buffer.
constexpr uint16_t AUDIO_BUFFER_SIZE_IN_SAMPLES = SAMPLES_PER_CYCLE * AUDIO_BUFFER_SIZE_IN_CYCLES;
// 4,000 bytes per buffer.
constexpr uint32_t AUDIO_BUFFER_SIZE_IN_BYTES = uint32_t(AUDIO_BUFFER_SIZE_IN_SAMPLES * (BITS_PER_SAMPLE / 8));

// The actual audio buffer
std::array<byte, AUDIO_BUFFER_SIZE_IN_BYTES> audio_buffer = {};

int init_xAudio2() {
    // #1 - Initialize COM
    // HRRESULT is for error checking
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Pointer to the main XAudio2 engine instance. This represents the core audio system 
    // object responsible for managing audio processing, creating voices, and handling audio 
    // playback on the system. It’s initialized to nullptr to indicate it hasn’t been created yet.
    IXAudio2* m_xAudio2 = nullptr; 
    // Pointer to the mastering voice, which is the final output voice in the XAudio2 system. 
    // It handles the audio output to the system’s audio device (e.g., speakers or headphones) 
    // and applies global audio effects or volume control before the sound is played. It’s 
    // initialized to nullptr until created.
    IXAudio2MasteringVoice* m_masteringVoice = nullptr; 
    // Pointer to a source voice, which is used to play a specific audio buffer or stream 
    // (e.g., a sound effect or music track). It manages the playback, pitch, volume, and 
    // other properties of an individual audio source, feeding data into the mastering 
    // voice for output. It’s initialized to nullptr until set up for a specific sound.
    // This handles the threading in the XAudio2 api
    IXAudio2SourceVoice* m_pXAudio2SourceVoice = nullptr; 

	if (SUCCEEDED(hr)) {
        // #2 - Create the XAudio2 engine object
        hr = ::XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr)) {
            // Clean up COM
            ::CoUninitialize(); 
            return -1;        
        }

        // #3 - Create a mastering voice (connect to the default audio device)
        hr = m_xAudio2->CreateMasteringVoice(
            &m_masteringVoice,              // Pointer to receive the mastering voice
            XAUDIO2_DEFAULT_CHANNELS,       // Use default number of channels (e.g., stereo)
            XAUDIO2_DEFAULT_SAMPLERATE,     // Use default sample rate (e.g., 44.1 kHz)
            0,                              // No flags
            nullptr,                        // No device ID (use default device)
            nullptr                         // No effect chain
        );
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

        // Define a format
        WAVEFORMATEX wave_format_ex = {};
        wave_format_ex.wFormatTag = WAVE_FORMAT_PCM;
        wave_format_ex.nChannels = 1; // 1 channel
        wave_format_ex.nSamplesPerSec = SAMPLES_PER_SEC;
        wave_format_ex.nBlockAlign = (wave_format_ex.nChannels * BITS_PER_SAMPLE) / 8;
        wave_format_ex.nAvgBytesPerSec = wave_format_ex.nSamplesPerSec * wave_format_ex.nBlockAlign;
        wave_format_ex.wBitsPerSample = BITS_PER_SAMPLE;
        wave_format_ex.cbSize = 0;

        // CreateSourceVoice allows you to play multiple independent sounds simultaneously 
        // (e.g., background music, sound effects, or tones) by creating multiple source voices, 
        // each with its own configuration.
        // XAudio2 makes an internal copy of the buffer when you submit it.
        hr = m_xAudio2->CreateSourceVoice(&m_pXAudio2SourceVoice, &wave_format_ex);
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

		double phase = {};
        uint32_t buffer_index = {};
        while (buffer_index < AUDIO_BUFFER_SIZE_IN_BYTES)
        {
            phase += (2 * PI) / SAMPLES_PER_CYCLE;
            int16_t sample = (int16_t)(sin(phase) * INT16_MAX * VOLUME);
            // Values are little-endian.
            audio_buffer[buffer_index++] = (byte)sample; 
            audio_buffer[buffer_index++] = (byte)(sample >> 8);
        }

		XAUDIO2_BUFFER x_audio_2_buffer = {};
        x_audio_2_buffer.Flags = XAUDIO2_END_OF_STREAM;
        x_audio_2_buffer.AudioBytes = AUDIO_BUFFER_SIZE_IN_BYTES;
        x_audio_2_buffer.pAudioData = audio_buffer.data();
        x_audio_2_buffer.PlayBegin = 0;
        x_audio_2_buffer.PlayLength = 0;
        x_audio_2_buffer.LoopBegin = 0;
        x_audio_2_buffer.LoopLength = 0;
        x_audio_2_buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

        hr = m_pXAudio2SourceVoice->SubmitSourceBuffer(&x_audio_2_buffer);
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

        // This function returns immediately, but the audio continues playing independently.
        hr = m_pXAudio2SourceVoice->Start(0);
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

        return 0; 
    }
    return -1;
}

void clean_up_audio() {
    ::CoUninitialize(); 
}

void read_chunck(Chunk_Type chunk_type, ) {
    if (chunk_type == )

}

void load_wav_file(const char* file_name) {
    // Read as raw bytes (8-bit value (0 - 255))
    FILE* file = fopen(file_name, "rb");

    fclose(file);
}
// play_sound
// pause_sound

