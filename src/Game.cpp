#include "Game.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

void draw_tile(MP_Renderer* renderer, Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}
	Image_Type type = IT_Rock_32x32;

	// Move everything around the camera
	int tile_ws_x = (Globals::tile_w * tile_index_x);
	int tile_ws_y = (Globals::tile_h * tile_index_y);

	int tile_cs_x = tile_ws_x - game_data.camera.x;
	int tile_cs_y = tile_ws_y - game_data.camera.y;

	float perlin_x = tile_index_x * noise_frequency;
	float perlin_y = tile_index_y * noise_frequency;
	float perlin = stb_perlin_noise3(perlin_x, perlin_y, 0, 0, 0, 0);

	if (perlin <= -0.2f) {
		type = IT_Water_32x32;
	} else if (perlin > -0.2f && perlin < 0.3f) {
		type = IT_Grass_32x32;
	} else {
		type = IT_Rock_32x32;
	}

	Image* image = get_image(type);

	MP_Rect dst = {tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

	mp_render_copy(renderer, image->texture, NULL, &dst);
}

void render(MP_Renderer* renderer, Game_Data& game_data) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}

	int window_width = 0;
	int window_height = 0;
	get_window_size(renderer->open_gl.window_handle, window_width, window_height);
	MP_Rect viewport = { 0, 0, window_width, window_height};
	mp_render_set_viewport(renderer, &viewport);

	renderer->window_width= window_width;
	renderer->window_height = window_height;
	game_data.camera.w = renderer->window_width;
	game_data.camera.h = renderer->window_height;

	mp_set_render_draw_color(renderer, 155, 155, 155, 255);
	mp_set_render_draw_color(renderer, 155, 155, 155, 255);
	mp_render_clear(renderer);

	mp_render_clear(renderer);

	// Create a render scene function that has the follow:
	// 1) Render player 
	// 2) Render entities

	// Camera relative to the game_data.player
	game_data.camera.x = (int)((float)game_data.player.position_ws.x - (float)game_data.camera.w / 2.0f);
	game_data.camera.y = (int)((float)game_data.player.position_ws.y - (float)game_data.camera.h / 2.0f);

	// draw_tile_map(renderer);
	// Have everything move around the game_data.camera / player
	// The player is just an offset from the game_data.camera. Camera is always 0,0

	// Truncates by default
	int starting_tile_x = game_data.camera.x / Globals::tile_w;
	int starting_tile_y = game_data.camera.y / Globals::tile_h;

	int ending_tile_x = (game_data.camera.x + game_data.camera.w) / Globals::tile_w;
	int ending_tile_y = (game_data.camera.y + game_data.camera.h) / Globals::tile_h;

	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			draw_tile(renderer, game_data, tile_x, tile_y, Globals::noise_frequency);
		}
	}

	game_data.player.position_cs.x = (float)game_data.camera.w / 2.0f - game_data.player.w / 2;
	game_data.player.position_cs.y = (float)game_data.camera.h / 2.0f - game_data.player.h / 2;
	mp_set_texture_alpha_mod(game_data.player.image->texture, 255);

	MP_Rect player_rect = {(int)game_data.player.position_cs.x, (int)game_data.player.position_cs.y, game_data.player.w, game_data.player.h};
	mp_render_copy(renderer, game_data.player.image->texture, NULL, &player_rect);

	Font* font = get_font(game_data.selected_font);
	draw_mp_library_debug_images(renderer, *font, game_data.player.image->texture, Globals::toggle_debug_images);

	mp_render_present(renderer);
}

