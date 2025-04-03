#pragma once
#include <dsound.h>

extern IDirectSoundBuffer* global_secondary_buffer;

void init_direct_sound(HWND* window, DWORD samples_per_second, DWORD buffer_size);
