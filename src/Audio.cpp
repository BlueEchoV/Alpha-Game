#include "Audio.h"

// void mp_init_audio() {
// 	Initialize();
// }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

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
