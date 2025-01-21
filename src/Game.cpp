#include "Game.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

void draw_circle(MP_Renderer* renderer, Color c, V2 pos_ws, V2 camera_pos, int radius_size, float total_lines) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}

	V2 pos_cs = convert_to_camera_space(camera_pos, pos_ws);

	int radius = radius_size;

	mp_set_render_draw_color(renderer, c);

	float increment_angle = 360.0f / total_lines;
	float current_degrees = 0;
	for (int i = 0; i < total_lines; i++) {
		float current_radians = convert_degrees_to_radians(current_degrees);
		float force_x = cos(current_radians);
		float force_y = sin(current_radians);
		float x1 = force_x * radius;
		float y1 = force_y * radius;
		current_degrees += increment_angle;

		current_radians = convert_degrees_to_radians(current_degrees);
		force_x = cos(current_radians);
		force_y = sin(current_radians);
		float x2 = force_x * radius;
		float y2 = force_y * radius;

		x1 += pos_cs.x;
		y1 += pos_cs.y;

		x2 += pos_cs.x;
		y2 += pos_cs.y;
		mp_render_draw_line(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
	}
}

void draw_tile(MP_Renderer* renderer, Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}
	Image_Type type = IT_Rock_32x32;

	// Move everything around the camera
	int tile_ws_x = (Globals::tile_w * tile_index_x);
	int tile_ws_y = (Globals::tile_h * tile_index_y);

	int tile_cs_x = tile_ws_x - (int)game_data.camera.pos_ws.x;
	int tile_cs_y = tile_ws_y - (int)game_data.camera.pos_ws.y;

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

void fire_arrow(Game_Data& game_data, Image_Type it, int speed, V2 target, V2 origin) {
	Image* image = get_image(it);

	V2 pos = origin;

	// Center the image
	pos.x -= image->w / 2;
	pos.y -= image->h / 2;

	// Calculate the direction vector target <--- origin
	V2 vel =  target - origin;

	// Calculate length
	float length = vel.x * 2 + vel.y * 2;
	length = sqrt(length);

	// Normalize the vel vector
	vel.x = vel.x / length;
	vel.y = vel.y / length;

	Arrow result = create_arrow(image, pos, vel, speed);

	game_data.arrows.push_back(result);
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
	mp_render_clear(renderer);

	mp_render_clear(renderer);

	// Camera relative to the game_data.player
	game_data.camera.pos_ws.x = ((float)game_data.player.position_ws.x - ((float)game_data.camera.w / 2.0f));
	game_data.camera.pos_ws.y = ((float)game_data.player.position_ws.y - ((float)game_data.camera.h / 2.0f));
	// 
	game_data.camera.pos_ws.x += game_data.player.w / 2;
	game_data.camera.pos_ws.y += game_data.player.h / 2;
	// Truncates by default
	int starting_tile_x = (int)game_data.camera.pos_ws.x / Globals::tile_w;
	int starting_tile_y = (int)game_data.camera.pos_ws.y / Globals::tile_h;

	int ending_tile_x = ((int)game_data.camera.pos_ws.x + game_data.camera.w) / Globals::tile_w;
	int ending_tile_y = ((int)game_data.camera.pos_ws.y + game_data.camera.h) / Globals::tile_h;

	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			draw_tile(renderer, game_data, tile_x, tile_y, Globals::noise_frequency);
		}
	}

	mp_set_texture_alpha_mod(game_data.player.image->texture, 255);

	// Convert the player's position to camera space
	MP_Rect player_rect = {(int)game_data.player.position_ws.x, (int)game_data.player.position_ws.y, game_data.player.w, game_data.player.h};
	V2 player_pos = convert_to_camera_space(game_data.camera.pos_ws, { (float)player_rect.x, (float)player_rect.y });
	player_rect.x = (int)player_pos.x;
	player_rect.y = (int)player_pos.y;
	mp_render_copy(renderer, game_data.player.image->texture, NULL, &player_rect);

	Font* font = get_font(game_data.selected_font);
	draw_mp_library_debug_images(renderer, *font, game_data.player.image->texture, Globals::toggle_debug_images);

	for (Arrow& arrow : game_data.arrows) {
		draw_arrow(renderer, (int)game_data.camera.pos_ws.x, (int)game_data.camera.pos_ws.y, arrow);
	}

	draw_circle(renderer, C_Green, { 500, 500 }, game_data.camera.pos_ws, 100, 10);

	mp_render_present(renderer);
}

