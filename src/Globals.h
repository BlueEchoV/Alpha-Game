#pragma once

// struct MP_Renderer;

struct MP_Renderer;

namespace Globals {
	// POTENTIAL RACE CONDITION IN MULTITHREADING
	extern MP_Renderer* renderer;

	extern int resolution_x;
	extern int resolution_y;
	extern bool toggle_debug_images;
	extern bool debug_show_coordinates;
	extern bool debug_show_stats;
	extern bool debug_show_colliders;

	extern float noise_frequency;

	extern int tile_w;
	extern int tile_h;

	extern int debug_total_arrows;

	extern float slow_frames_per_sec;
	extern float fast_frames_per_sec;
	extern float travel_speed_based_frames_per_second;

	const int MAX_COLLIDERS = 100;
	const int DEFAULT_HEALTH_BAR_WIDTH = 60;
	const int DEFAULT_HEALTH_BAR_HEIGHT = 8;
	const int DEFAULT_HEALTH_BAR_OUTLINE = 3;
}
