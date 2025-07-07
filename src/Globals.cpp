#include "Globals.h"

namespace Globals {
	MP_Renderer* renderer = {};
	
	extern bool running = true;

	int resolution_x = 1600;
	int resolution_y = 900;

	bool toggle_debug_images = false;
	bool toggle_debug_unit_images = true;
	bool debug_show_coordinates = false;
	bool debug_show_stats = false;
	bool debug_show_colliders = false;

	float noise_frequency = 0.05f;
	// Pixels
	int tile_w = 32;
	int tile_h = 32;

	extern int debug_total_arrows = 0;

	extern float slow_frames_per_sec = 8;
	extern float fast_frames_per_sec = 16;
	extern float travel_speed_based_frames_per_second;

}
