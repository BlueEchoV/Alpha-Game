#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"
#include "Globals.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void draw_tile(MP_Renderer* renderer, Game_Data& game_data, int tile_index_x, int tile_index_y) {
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

	int hash = tile_ws_x * 73856093 ^ tile_ws_y * 19349663;
	switch (hash % (TT_Total)) {
	case 0: {
		type = IT_Rock_32x32;
		break;
	}
	case 1: {
		type = IT_Grass_32x32;
		break;
	}
	case 2: {
		type = IT_Water_32x32;
		break;
	}
	default: {
		log("Error: Default case hit in tile generation");
		type = IT_Rock_32x32;
		break;
	}
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
			draw_tile(renderer, game_data, tile_x, tile_y);
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