#include "World.h"

#include <perlin.h>

V2 get_tile_pos_ws(int tile_index_x, int tile_index_y) {
	return {(float)Globals::tile_w * (float)tile_index_x, (float)Globals::tile_h * (float)tile_index_y};
}

V2 get_tile_pos_index(V2 pos_ws) {
	return { pos_ws.x / Globals::tile_w, pos_ws.y / Globals::tile_h };
}

// All tilemaps are centered around 0, 0 ws position
Tile_Map create_tile_map(int w_in_tiles, int h_in_tiles) {
	Tile_Map result = {};

	result.w_in_tiles = w_in_tiles;
	result.h_in_tiles = h_in_tiles;

	result.w_in_pixels = result.w_in_tiles * Globals::tile_w;
	result.h_in_pixels = result.h_in_tiles * Globals::tile_h;

	assert(w_in_tiles % 2 == 0);
	assert(h_in_tiles % 2 == 0);

	// NOTE: If the width or height is odd, this gets weird
	result.left_ws = -((result.w_in_pixels) / 2);
	// Account for the additional tile (origin is bottom left corner
	result.top_ws = (result.h_in_pixels) / 2;
	// Account for the additional tile (origin is bottom left corner
	result.right_ws = (result.w_in_pixels) / 2;
	result.bottom_ws = -((result.h_in_pixels) / 2);

	return result;
}

World create_world(int w, int h, MP_Texture* tex_1, MP_Texture* tex_2, MP_Texture* noise_tex) {
	World result = {};

	result.map = create_tile_map(w, h);
	result.tex_1 = tex_1;
	result.tex_2 = tex_2;
	result.noise_tex = noise_tex;
	
	return result;
}

void draw_tile(std::string tile_type, Camera camera, int tile_index_x, int tile_index_y) {
	std::string type = tile_type;
	Image* image = get_image(type);

	// Move everything around the camera
	int tile_ws_x = (Globals::tile_w * tile_index_x);
	int tile_ws_y = (Globals::tile_h * tile_index_y);

	int tile_cs_x = tile_ws_x - (int)camera.pos_ws.x;
	int tile_cs_y = tile_ws_y - (int)camera.pos_ws.y;

	MP_Rect dst = {tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

	mp_render_copy(image->texture, NULL, &dst);
}

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
		type = "IT_Water_32x32";
	} else if (perlin > -0.2f && perlin < 0.3f) {
		type = "IT_Grass_32x32";
	} else {
		type = "IT_Rock_32x32";
	}

	Image* image = get_image(type);

	MP_Rect dst = {tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

	mp_render_copy(image->texture, NULL, &dst);
}

void draw_perlin_tile_map(Camera& camera, Tile_Map& tile_map) {
	// Clear the background to black
	MP_Rect rect = {0, 0, Globals::playground_area_w, Globals::playground_area_h};
	mp_set_render_draw_color(CT_Black);
	mp_render_fill_rect(&rect);

	// Truncates by default
	// The -1 for the range of -63 to 64. It counts 0 as a negative number
	int starting_tile_x = ((int)camera.pos_ws.x / Globals::tile_w) - 1;
	int starting_tile_y = ((int)camera.pos_ws.y / Globals::tile_h) - 1;

	int ending_tile_x = ((int)camera.pos_ws.x + camera.w) / Globals::tile_w;
	int ending_tile_y = ((int)camera.pos_ws.y + camera.h) / Globals::tile_h;

	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			// Center the world on 0, 0
			if (tile_x < -((tile_map.w_in_tiles / 2)) || 
				tile_x >  (tile_map.w_in_tiles / 2) || 
				tile_y < -((tile_map.h_in_tiles / 2)) ||
				tile_y >  (tile_map.h_in_tiles / 2)) {
				continue;
			}
			draw_perlin_tile(camera, tile_x, tile_y, Globals::noise_frequency);
		}
	}
}

// Pass in the tilemap here?
void draw_environment_entity(Camera camera, int tile_index_x, int tile_index_y, Environment_Entity ev_type) {
	// ONLY DRAW WHAT IS INSIDE OF THE CAMERA'S RANGE
	// The map is draw in the center of the world
	std::string image_name = "";
	
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
	case EE_Bush: {
		image_name = "bush";
		break;
	}
	default: {
		image_name = "";
		break;
	}
	}

	// Only draw if we pull an image to draw
	if (image_name != "") {
		V2 tile_pos_ws = get_tile_pos_ws(tile_index_x, tile_index_y);

		// Move everything around the camera
		int tile_cs_x = (int)tile_pos_ws.x - (int)camera.pos_ws.x;
		int tile_cs_y = (int)tile_pos_ws.y - (int)camera.pos_ws.y;

		Image* image = get_image(image_name);

		MP_Rect dst = { tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

		mp_render_copy(image->texture, NULL, &dst);
	}
}

// The entire map is techinically an offset of the player
void draw_entire_world(Camera& camera, World& world) {	
	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	// draw_perlin_tile_map(camera, tile_map);

	// Clear the background to black
	MP_Rect rect = {0, 0, Globals::playground_area_w, Globals::playground_area_h};
	mp_set_render_draw_color(CT_Black);
	mp_render_fill_rect(&rect);

	// Draws around the player
	mp_draw_blended_perlin_tile_map_around_player(camera, world.tex_1, world.tex_2, world.noise_tex);

	// Truncates by default
	// The -1 for the range of -63 to 64. It counts 0 as a negative number
	int starting_tile_x = ((int)camera.pos_ws.x / Globals::tile_w) - 1;
	int starting_tile_y = ((int)camera.pos_ws.y / Globals::tile_h) - 1;

	int ending_tile_x = ((int)camera.pos_ws.x + camera.w) / Globals::tile_w;
	int ending_tile_y = ((int)camera.pos_ws.y + camera.h) / Globals::tile_h;

	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			float perlin_x = tile_x * Globals::noise_frequency;
			float perlin_y = tile_y * Globals::noise_frequency;
			float perlin = stb_perlin_noise3(perlin_x, perlin_y, 0, 0, 0, 0);

			Environment_Entity ee_type = EE_Empty;
			if (perlin > 0.0f) {
				ee_type = EE_Bush;
			} else {
				ee_type = EE_Rock;
			}

			// Center the world on 0, 0
			if (tile_x < -((world.map.w_in_tiles / 2)) || 
				// The -1 is to account for the range -64 to +63
				tile_x >  (world.map.w_in_tiles / 2) - 1 || 
				tile_y < -((world.map.h_in_tiles / 2)) ||
				tile_y >  (world.map.h_in_tiles / 2) - 1) {
				draw_environment_entity(camera, tile_x, tile_y, ee_type);
			}
		}
	}
}
