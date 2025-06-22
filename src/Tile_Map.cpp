#include "Tile_Map.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

V2 get_tile_pos_ws(int tile_index_x, int tile_index_y) {
	return {(float)Globals::tile_w * (float)tile_index_x, (float)Globals::tile_h * (float)tile_index_y};
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

void draw_perlin_tile_map(Camera camera) {
	// Clear the background to black
	MP_Rect rect = {0, 0, Globals::resolution_x, Globals::resolution_y };
	mp_set_render_draw_color(CT_Black);
	mp_render_fill_rect(&rect);

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
			// Center the world on 0, 0
			if (tile_x < -(Globals::WORLD_SIZE_W / 2) || 
				tile_x >  (Globals::WORLD_SIZE_W / 2) || 
				tile_y < -(Globals::WORLD_SIZE_H / 2) ||
				tile_y >  (Globals::WORLD_SIZE_H / 2)) {
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

float horizontal_radius = 20;
float vertical_radius = 20;

V2 center_of_map_ws = { 0, 0 };

void draw_ellipse_perlin_environment_tile(Camera camera, int tile_index_x, int tile_index_y) {
// Default to blocked
    Environment_Entity ev = EE_Bush;

    // Calculate the center of the map in tile coordinates
    float center_tile_x = center_of_map_ws.x / Globals::tile_w;
    float center_tile_y = center_of_map_ws.y / Globals::tile_h;

    // Define the radius for the square (assuming horizontal_radius is available)
    // For a square, horizontal and vertical extents are equal
    float radius = horizontal_radius; // Use a single radius value for simplicity

    // Calculate normalized distances from the center
    float dx = (tile_index_x - center_tile_x) / radius;
    float dy = (tile_index_y - center_tile_y) / radius;

    // Sample Perlin noise for content variation inside the square
    float frequency = 0.15f;
    float noise = (stb_perlin_noise3(tile_index_x * frequency, tile_index_y * frequency, 100, 0, 0, 0) + 1.0f) / 2.0f;

    // Check if the tile is inside the square
    if (fabs(dx) < 1.0f && fabs(dy) < 1.0f) {
        // Inside the square
        if (noise > 0.4f) {
            ev = EE_Empty;  // Walkable tile
        } else {
            ev = EE_Bush;   // Blocked tile
        }
    } else {
        // Outside the square
        ev = EE_Bush;
    }

    // Draw the tile
    draw_environment_entity(camera, tile_index_x, tile_index_y, ev);
}

int walkable_region_w_in_tiles = 30;
int walkable_region_h_in_tiles = 20;

/*
void draw_perlin_square_environment_entities(Camera camera, V2 center, int tile_index_x, int tile_index_y) {
    Environment_Entity ev = EE_Empty; 

    float center_tile_x = center_of_map_ws.x / Globals::tile_w;
    float center_tile_y = center_of_map_ws.y / Globals::tile_h;

	if (tile_index_x < ((int)center_tile_x - walkable_region_w_in_tiles / 2 ) ||	// Left boundary
		tile_index_x > ((int)center_tile_x + walkable_region_w_in_tiles / 2)  ||	// Right boundary
		tile_index_y < ((int)center_tile_y - walkable_region_h_in_tiles / 2)  ||	// Bottom boundary
		tile_index_y > ((int)center_tile_y + walkable_region_h_in_tiles / 2))		// Top boundary
	{  
		float noise = (stb_perlin_noise3(tile_index_x * Globals::noise_frequency, tile_index_y * Globals::noise_frequency, 100, 0, 0, 0) + 1.0f) / 2.0f;

		draw_environment_entity(camera, tile_index_x, tile_index_y, ev);
	}
}
*/

// The entire map is techinically an offset of the player
void draw_entire_map(Camera camera) {
	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	draw_perlin_tile_map(camera);

#if 0
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			/*
			if (
				// Left boundary
				tile_x < ((int)center_of_map_ws.x - walkable_region_w_in_tiles / 2 ) || 
				// Right boundary
				tile_x > ((int)center_of_map_ws.x + walkable_region_w_in_tiles / 2) || 
				// Bottom boundary
				tile_y < ((int)center_of_map_ws.y - walkable_region_h_in_tiles / 2) || 
				// Top Boundary
				tile_y > ((int)center_of_map_ws.x + walkable_region_h_in_tiles / 2)) {
				draw_environment_entity(camera, tile_x, tile_y, EE_Bush);
			}
			*/
			draw_ellipse_perlin_environment_tile(camera, tile_x, tile_y);
		}
	}
#endif
}
