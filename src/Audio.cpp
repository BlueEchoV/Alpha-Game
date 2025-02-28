#include "Audio.h"

#include <windows.h>
#include <stdexcept>
#include <xaudio2.h>
#include <array>

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

std::array<byte, AUDIO_BUFFER_SIZE_IN_BYTES> audio_buffer = {};

int init_x_audio_2() {
    // #1 - Initialize COM
    // HRRESULT is for error checking
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IXAudio2* m_xAudio2 = nullptr; 
    IXAudio2MasteringVoice* m_masteringVoice = nullptr;
	if (SUCCEEDED(hr)) {
        // #2 - Create the XAudio2 engine object
        hr = ::XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr)) {
            // Clean up COM
            ::CoUninitialize(); 
            return -1;        }

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
            // Clean up XAudio2
            if (m_xAudio2) m_xAudio2->Release(); 
            // Clean up COM
            ::CoUninitialize(); 
            return -1; 
        }

        // Define a format
        WAVEFORMATEX wave_format_ex = {};
        wave_format_ex.wFormatTag = WAVE_FORMAT_PCM;
        wave_format_ex.nChannels = 1; // 1 channel
        wave_format_ex.nSamplesPerSec = SAMPLES_PER_SEC;


        return 0; // Success!
    }
    return -1;
}

void clean_up_audio() {
    ::CoUninitialize(); 
}

/* 
void play_audio_stream() {
	HRESULT error_codes;

	// This creates an instance of the device enumerator, 
	// which allows the program to find audio devices.
	error_codes = CoCreateInstance(
		CLSID_MMDeviceEnumerator, 
		NULL,
		CLSCTX_ALL, 
		IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	// TODO: Error check here

}

// void mp_play_audio() 
// void mp_stop_audio() 

void test() {
	// HRESULT temp = CoCreateInstance();
}
*/
