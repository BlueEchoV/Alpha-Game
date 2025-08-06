#pragma once

// struct MP_Renderer;

struct MP_Renderer;
namespace Globals {
	// POTENTIAL RACE CONDITION IN MULTITHREADING
	extern MP_Renderer* renderer;

	extern bool running;

	// NOTE: This size INCLUDES the header bar of the window. The white bar 
	// that contains the name of the game, the close X button, the minimize,
	// etc.
	extern int entire_window_w;
	extern int entire_window_h;

	extern int client_area_w;
	extern int client_area_h;
	// Actual game area. Never change this.
	extern int playground_area_w;
	extern int playground_area_h;
	// For calculating the viewport
	extern float playground_area_target_aspect_ratio;
	extern bool viewport_needs_update;

	extern int active_viewport_x;
	extern int active_viewport_y;
	extern int active_viewport_h;
	extern int active_viewport_w;
	extern float active_viewport_scale_x;
	extern float active_viewport_scale_y;

	extern bool toggle_debug_images;
	extern bool toggle_debug_unit_images;
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
