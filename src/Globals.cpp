#include "Globals.h"

namespace Globals {
	MP_Renderer* renderer = {};
	
	extern bool running = true;

	// NOTE: THIS DETERMINES THE PLAYGROUND AREA
	extern int entire_window_w = 1600;
	extern int entire_window_h = 900;

	extern int client_area_w = 0;
	extern int client_area_h = 0;
	// Never change this value.
	extern int playground_area_w = 0;
	extern int playground_area_h = 0;
	extern float playground_area_target_aspect_ratio = 0.0f;
	extern bool viewport_needs_update = false;

	extern int active_viewport_x = 0;
	extern int active_viewport_y = 0;
	extern int active_viewport_h = 0;
	extern int active_viewport_w = 0;
	extern float active_viewport_scale_x = 0;
	extern float active_viewport_scale_y = 0;

	// NOTE: WILL BE REPLACED WITH A CONSOLE
	bool toggle_debug_images = false;
	bool toggle_debug_unit_images = false;
	bool debug_show_coordinates = false;
	bool debug_show_stats = false;
	bool debug_show_colliders = false;
	bool debug_show_wireframes = false;
	bool debug_show_hovered_tile = false;

	float noise_frequency = 0.05f;
	// Pixels
	int tile_w = 32;
	int tile_h = 32;

	extern int debug_total_arrows = 0;

	extern float slow_frames_per_sec = 8;
	extern float fast_frames_per_sec = 16;
	extern float travel_speed_based_frames_per_second;
}
