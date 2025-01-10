#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"
#include "Globals.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

	// *********Visualization code****************
	mp_set_render_draw_color(renderer, C_Orange);
	V2_F c = {};
	c.x = game_data.player.position_cs.x + 400;
	c.y = game_data.player.position_cs.y;

	int w = 150;
	int h = 150;
	static MP_Point top_left =	   { (int)c.x - (w / 2), (int)c.y + (h / 2)};
	static MP_Point top_right =    { (int)c.x + (w / 2), (int)c.y + (h / 2)};
	static MP_Point bottom_right = { (int)c.x + (w / 2), (int)c.y - (h / 2)};
	static MP_Point bottom_left =  { (int)c.x - (w / 2), (int)c.y - (h / 2)};

	static float angle = 0.0f;
	static float last_angle = 0.0f;
	float rotation_speed = 1.0f;
	if (key_pressed(KEY_R)) {
		angle += rotation_speed;
	}
	if (key_pressed(KEY_F)) {
		angle -= rotation_speed;
	}
	if (angle != last_angle) {
		V2_F top_left_v2 = rotate_point_based_off_angle(angle, c.x, c.y, (float)top_left.x, (float)top_left.y);
		top_left.x = (int)top_left_v2.x;
		top_left.y = (int)top_left_v2.y;
		V2_F top_right_v2 = rotate_point_based_off_angle(angle, c.x, c.y, (float)top_right.x, (float)top_right.y);
		top_right.x = (int)top_right_v2.x;
		top_right.y = (int)top_right_v2.y;
		V2_F bottom_right_v2 = rotate_point_based_off_angle(angle, c.x, c.y, (float)bottom_right.x, (float)bottom_right.y);
		bottom_right.x = (int)bottom_right_v2.x;
		bottom_right.y = (int)bottom_right_v2.y;
		V2_F bottom_left_v2 = rotate_point_based_off_angle(angle, c.x, c.y, (float)bottom_left.x, (float)bottom_left.y);
		bottom_left.x = (int)bottom_left_v2.x;
		bottom_left.y = (int)bottom_left_v2.y;
		last_angle = angle;
	} 	
	int string_size = 1;
	std::string center_string = {};
	center_string = "(" + std::to_string(c.x) + " " + std::to_string(c.y) + ")";
	draw_string(renderer, *font, center_string.c_str(), (int)c.x, (int)c.y, string_size, true, false);

	std::string angle_string = {};
	center_string = std::to_string(angle);
	draw_string(renderer, *font, center_string.c_str(), (int)c.x, (int)c.y + (font->char_height * 2) * string_size, string_size, true, false);

	std::string top_left_string = {};
	top_left_string = "(" + std::to_string(top_left.x) + " " + std::to_string(top_left.y) + ")";
	draw_string(renderer, *font, top_left_string.c_str(), top_left.x, top_left.y, string_size, true, false);

	std::string top_right_string = {};
	top_right_string = "(" + std::to_string(top_right.x) + " " + std::to_string(top_right.y) + ")";
	draw_string(renderer, *font, top_right_string.c_str(), top_right.x, top_right.y, string_size, true, false);

	std::string bottom_right_string = {};
	bottom_right_string = "(" + std::to_string(bottom_right.x) + " " + std::to_string(bottom_right.y) + ")";
	draw_string(renderer, *font, bottom_right_string.c_str(), bottom_right.x, bottom_right.y, string_size, true, false);

	std::string bottom_left_string = {};
	bottom_left_string = "(" + std::to_string(bottom_left.x) + " " + std::to_string(bottom_left.y) + ")";
	draw_string(renderer, *font, bottom_left_string.c_str(), bottom_left.x, bottom_left.y, string_size, true, false);

	mp_render_draw_line(renderer, top_left.x,	  top_left.y,     top_right.x,    top_right.y);
	mp_render_draw_line(renderer, top_right.x,	  top_right.y,    bottom_right.x, bottom_right.y);
	mp_render_draw_line(renderer, bottom_right.x, bottom_right.y, bottom_left.x,  bottom_left.y);
	mp_render_draw_line(renderer, bottom_left.x,  bottom_left.y,  top_left.x,     top_left.y);
	// *******************************************

	mp_render_present(renderer);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	MP_Renderer* renderer = mp_create_renderer(hInstance);
	load_images(renderer);
	load_fonts(renderer);

	Game_Data game_data = {};
	game_data.selected_font = "basic_font";

	int player_speed = 1;
	game_data.player = create_player(get_image(IT_Dummy_Player), player_speed);

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	get_window_size(renderer->open_gl.window_handle, game_data.camera.w, game_data.camera.h);
	game_data.player.w = 200;
	game_data.player.h = 200;

	bool running = true;
	while (running) {
		reset_is_pressed();

		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) {
				running = false;
			}
		}

		// input()
		if (key_pressed_and_held(KEY_W)) {
			game_data.player.position_ws.y += player_speed;
		}
		if (key_pressed_and_held(KEY_S)) {
			game_data.player.position_ws.y -= player_speed;
		}
		if (key_pressed_and_held(KEY_D)) {
			game_data.player.position_ws.x += player_speed;
		}
		if (key_pressed_and_held(KEY_A)) {
			game_data.player.position_ws.x -= player_speed;
		}
		if (key_pressed(VK_OEM_3)) {
			Globals::toggle_debug_images = !Globals::toggle_debug_images;
		}

		// Update

		// Render
		render(renderer, game_data);
	}

	ReleaseDC(renderer->open_gl.window_handle, renderer->open_gl.window_dc);
	return 0;
}