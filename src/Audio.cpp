#include "Audio.h"

#include <windows.h>
#include <stdexcept>
#include <xaudio2.h>

int init_x_audio_2() {
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IXAudio2* m_xAudio2 = nullptr; 
    IXAudio2MasteringVoice* m_masteringVoice = nullptr;
	if (SUCCEEDED(hr)) {
        // Create the XAudio2 engine
        hr = ::XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr)) {
            // Clean up COM
            ::CoUninitialize(); 
            return -1;        }

        // Create a mastering voice (connect to the default audio device)
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

        return 0; // Success!
    }
    return -1;
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
