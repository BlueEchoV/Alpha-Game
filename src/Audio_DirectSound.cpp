#include "Audio_DirectSound.h"

#include <dsound.h>

IDirectSoundBuffer* global_secondary_buffer = {};

// Review
// ************************************************************************
// SAMPLE: In audio programming, a sample is a tiny snapshot of sound taken 
//		   at a specific moment in time. Think of it like a single frame in a 
//		   movie—it’s just one small piece of the audio that, when combined with 
//		   many other samples, creates the full sound you hear, like a song or a voice.
// COM: Think of COM as a vending machine system, and DirectSound as a specific vending machine for audio:
//      Without COM: You’d have to manually find the vending machine’s instruction manual (using GetProcAddress) 
//          every time you want to buy a snack. For example, you’d look up “How to get a soda,” then “How to get chips,” 
//          and so on. This is slow and tedious.
//      With COM: When you get the vending machine (DirectSoundCreate gives you the DirectSound object), it comes with 
//          a built-in menu (the vtable). The menu already lists all the snacks (functions like CreateSoundBuffer) 
//          and their buttons (addresses). You just press the button for “soda” (call the function), and the machine 
//          knows what to do.
// ************************************************************************

/* 
    The below function does the follow:
        1. Load dsound.dll library.
        2. Get the address of the function DirectSoundCreate.
           - This is the only function that we need to pull directly from dsound.dll.
        3. Create DirectSound object.
        4. Set up the sound format. Variable frequency aside, we know that our sound will have the following properties:
           - Stereo (2 channels)
           - 16 bits per sample (Audio CD quality)
           - We also calculate other things required by the API
           - We requested the sound to be of 48kHz because this seems to be the most common native format for sound output. 
             You can also set it to be 44.1kHz, another common format and the one used in Audio CDs.
        5. Set the “cooperative level” of our window to be priority.
           - This will allow us to later set up the format of the primary buffer.
        6. Create Primary Buffer.
           - In order to do that, we need to describe our buffer in the DSBUFFERDESC structure.
        7. Set the format of the buffer based on the WAVEFORMATEX structure we defined in step 4.
           - We really only needed to create primary buffer for this.
           - In theory, you could skip steps 5-7, but then you don't have guarantee that your sound wouldn't be up/downsampled 
              by Windows.
        8. Create Secondary Buffer.
           - We need a buffer description here as well. Among other things we pass a buffer size. We want our buffer to 
             be 2 seconds long to avoid bad jumps on skipped frames.
           - We don't need to set the format separately here, as it can be passed directly inside BufferDescription.
    NOTE: The secondary buffer is the only thing we will be using at this point.
*/

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

        // GetProcAddress is like finding the vending machine’s address in a directory. You only need to do this once 
        //      to get the machine (DirectSoundCreate).
        // After that, the machine’s menu (vtable) is already set up in memory (loaded with the dsound.dll). 
        //      The DirectSound object points to this menu, so you can press buttons (call functions) directly. 
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
                        // NOTE: We have finally set the format of the primary buffer!
                         OutputDebugStringA("Primary buffer format was set.\n");
                    }
                }
            }

            // The second buffer is added because the first buffer (primary) controls 
            //      the main audio output, while the secondary buffer holds specific sounds you want to play, 
            //      like music or sound effects. It’s like having a main speaker system (primary) and a separate 
            //      playlist (secondary) you can control.
            // NOTE: "Create" a secondary buffer
			DSBUFFERDESC buffer_description = {};
			buffer_description.dwSize = sizeof(buffer_description);      
			buffer_description.dwBufferBytes = buffer_size;
			buffer_description.lpwfxFormat = &wave_format;

			if(SUCCEEDED(direct_sound->CreateSoundBuffer(&buffer_description, &global_secondary_buffer, 0)))
			{
				// NOTE: All good, secondary buffer works as intended
                OutputDebugStringA("Secondary buffer created Successfully.\n");
			}
        }
    }
}