#include "Tile_Map.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

void draw_tile(Camera camera, int tile_index_x, int tile_index_y, float noise_frequency) {
	std::string type = "IT_Rock_32x32";

	// Move everything around the camera
	int tile_ws_x = (Globals::tile_w * tile_index_x);
	int tile_ws_y = (Globals::tile_h * tile_index_y);

	int tile_cs_x = tile_ws_x - (int)camera.pos_ws.x;
	int tile_cs_y = tile_ws_y - (int)camera.pos_ws.y;

	float perlin_x = tile_index_x * noise_frequency;
	float perlin_y = tile_index_y * noise_frequency;
	float perlin = stb_perlin_noise3(perlin_x, perlin_y, 0, 0, 0, 0);

	if (perlin <= -0.2f) {
		// type = "IT_Water_32x32";
		type = "IT_Grass_32x32";
	} else if (perlin > -0.2f && perlin < 0.3f) {
		type = "IT_Rock_32x32";
		// type = "IT_Grass_32x32";
	} else {
		type = "IT_Grass_32x32";
		// type = "IT_Rock_32x32";
	}

	Image* image = get_image(type);

	MP_Rect dst = {tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

	mp_render_copy(image->texture, NULL, &dst);
}

void draw_tile_map(Camera camera) {
	// Truncates by default
	int starting_tile_x = (int)camera.pos_ws.x / Globals::tile_w;
	int starting_tile_y = (int)camera.pos_ws.y / Globals::tile_h;

	int ending_tile_x = ((int)camera.pos_ws.x + camera.w) / Globals::tile_w;
	int ending_tile_y = ((int)camera.pos_ws.y + camera.h) / Globals::tile_h;

	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			draw_tile(camera, tile_x, tile_y, Globals::noise_frequency);
		}
	}
}
