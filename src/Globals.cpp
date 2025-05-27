#include "Globals.h"

namespace Globals {
	MP_Renderer* renderer = {};

	int resolution_x = 1600;
	int resolution_y = 900;

	bool toggle_debug_images = true;
	bool debug_show_coordinates = true;
	bool debug_show_stats = true;

	float noise_frequency = 0.05f;
	// Pixels
	int tile_w = 32;
	int tile_h = 32;

	int player_width = 192;
	int player_height = 192;

	extern int debug_total_arrows = 0;

	extern float default_animation_play_speed = 0.25f;
}
