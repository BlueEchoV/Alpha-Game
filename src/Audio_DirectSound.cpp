#include "Audio_DirectSound.h"

#include <dsound.h>

// Review:

// ************************************************************************
// SAMPLE: In audio programming, a sample is a tiny snapshot of sound taken 
//		   at a specific moment in time. Think of it like a single frame in a 
//		   movie—it’s just one small piece of the audio that, when combined with 
//		   many other samples, creates the full sound you hear, like a song or a voice.
// ************************************************************************

// This is like creating a reusable blueprint (a macro) in C/C++ programming. It defines a function signature 
//      called DIRECT_SOUND_CREATE. You can give it any name you want (like MyFunction), and it will turn into a function that:
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
// This gives the blueprint a specific name: Direct_Sound_Create. It’s like saying, “From now on, when I say 
//      Direct_Sound_Create, I mean this exact function setup.” It makes it easier to use later.
typedef DIRECT_SOUND_CREATE(Direct_Sound_Create);

void init_direct_sound(HWND* window, DWORD samples_per_second, DWORD buffer_size) {
    // "dsound.dll" is a library file (part of Windows) that helps programs work with sound, like playing audio or managing speakers.
    // LoadLibraryA is a function that grabs this file from the system and makes it available for the program to use.
    // HMODULE is just a way to store a reference to this loaded library, so the program can keep track of it.
    // Example: Like opening a the toolbox. We are borrowing a sound toolkit from Windows
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

    // Don't crash the game if the dll doesn't load. It is not a necessary part of the game.
    if (DSoundLibrary) {
		// NOTE: Create a DirectSound object 
        // This is like reaching into the toolbox (DSoundLibrary) you just opened and pulling out a specific tool 
        //      called "DirectSoundCreate".
		// GetProcAddress finds the exact function you want from the library by its name and gives you a pointer to it.
		// You’re assigning this tool to a variable called DIRECT_SOUND_CREATE, and you’re telling the program it
        //      matches the Direct_Sound_Create blueprint from earlier. Now you can use it to start working with sound in 
        //      your program.
        Direct_Sound_Create* DirectSoundCreate = (Direct_Sound_Create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        IDirectSound* direct_sound;
        if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &direct_sound, 0))) {
			WAVEFORMATEX wave_format= {};
            wave_format.wFormatTag = WAVE_FORMAT_PCM;
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = samples_per_second;
            wave_format.wBitsPerSample = 16;
            wave_format.nBlockAlign = (wave_format.nChannels * wave_format.wBitsPerSample) / 8; // 4 under current settings
            wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;

			// NOTE: Set cooperative level
            if (SUCCEEDED(direct_sound->SetCooperativeLevel(*window, DSSCL_PRIORITY)))
            {
                // NOTE: "Create" a primary buffer

                // This is like filling out a form that describes what kind of audio setup you want
                DSBUFFERDESC buffer_description= {};
                buffer_description.dwSize = sizeof(buffer_description);
                buffer_description.dwFlags = DSBCAPS_PRIMARYBUFFER;

                // Think of primary_buffer as the actual "speaker" that will play the sound
                IDirectSoundBuffer* primary_buffer;

                // This line asks the DirectSound system to set up the audio based on your "form" (BufferDescription)
                // DirectSound is the audio manager, and CreateSoundBuffer is like saying "please make me a speaker"
                if (SUCCEEDED(direct_sound->CreateSoundBuffer(&buffer_description, &primary_buffer, 0)))
                {

                    // SetFormat tells it how the sound should come out (e.g., stereo, mono, or how clear it should be).
					if (SUCCEEDED(primary_buffer->SetFormat(&wave_format)))
                    {
                        // NOTE(casey): We have finally set the format of the primary buffer!
                    }
                }
                // NOTE: "Create" a secondary buffer     
            }
        }
    }
}