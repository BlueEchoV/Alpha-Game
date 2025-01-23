#pragma once

// struct MP_Renderer;

struct MP_Renderer;

namespace Globals {
	// POTENTIAL RACE CONDITION IN MULTITHREADING
	extern MP_Renderer* renderer;

	extern int resolution_x;
	extern int resolution_y;
	extern bool toggle_debug_images;

	extern float noise_frequency;
	// Pixels
	extern int tile_w;
	extern int tile_h;

	extern int player_width;
	extern int player_height;
}
