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

void init_direct_sound() {
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
            int i = 0;
            i++;
			// NOTE: Set cooperative level
			// NOTE: "Create" a primary buffer
			// NOTE: "Create" a secondary buffer     
        }
    }
}