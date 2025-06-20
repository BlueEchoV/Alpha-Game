#include "Tile_Map.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

void draw_perlin_tile(Camera camera, int tile_index_x, int tile_index_y, float noise_frequency) {
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

void draw_perlin_tile_map(Camera camera) {
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
			draw_perlin_tile(camera, tile_x, tile_y, Globals::noise_frequency);
		}
	}
}


// Pass in the tilemap here?
void draw_environment_entity(Camera camera, int tile_index_x, int tile_index_y, Environment_Entity ev_type) {
	// ONLY DRAW WHAT IS INSIDE OF THE CAMERA'S RANGE
	// The map is draw in the center of the world
	std::string image_name = "";
	if (tile_index_x < 0 || 
		tile_index_x > (camera.w / Globals::tile_w) || 
		tile_index_y < 0 ||
		tile_index_y > (camera.h / Globals::tile_h)
		) {
		image_name = "tree";
	}

	switch (ev_type) {
	case EE_Empty: {
		// Do nothing
		image_name = "";
		break;
	}
	case EE_Tree: {
		image_name = "tree";
		break;
	}
	case EE_Rock: {
		image_name = "rock";
		break;
	}
	default: {
		image_name = "";
		break;
	}
	}

	// Only draw if we pull an image to draw
	if (image_name != "") {
		int tile_ws_x = (Globals::tile_w * tile_index_x);
		int tile_ws_y = (Globals::tile_h * tile_index_y);
		// Move everything around the camera
		int tile_cs_x = tile_ws_x - (int)camera.pos_ws.x;
		int tile_cs_y = tile_ws_y - (int)camera.pos_ws.y;

		Image* image = get_image(image_name);

		MP_Rect dst = { tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

		mp_render_copy(image->texture, NULL, &dst);
	}
}

int walkable_region_w_in_tiles = 20;
int walkable_region_h_in_tiles = 20;

V2 center_of_map_ws = { 0, 0 };
	
// The entire map is techinically an offset of the player
void draw_entire_map(Camera camera) {
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
			draw_perlin_tile(camera, tile_x, tile_y, Globals::noise_frequency);
		}
	}
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			if (
				// Left boundary
				tile_x < ((int)center_of_map_ws.x - walkable_region_w_in_tiles / 2 ) || 
				// Right boundary
				tile_x > ((int)center_of_map_ws.x + walkable_region_w_in_tiles / 2) || 
				// Bottom boundary
				tile_y < ((int)center_of_map_ws.y - walkable_region_h_in_tiles / 2) || 
				// Top Boundary
				tile_y > ((int)center_of_map_ws.x + walkable_region_h_in_tiles / 2)) {
				draw_environment_entity(camera, tile_x, tile_y, EE_Tree);
			}
		}
	}
}
