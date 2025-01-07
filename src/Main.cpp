#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"
#include "Globals.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void render(MP_Renderer* renderer, Game_Data& game_data) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}

	mp_set_render_draw_color(renderer, 155, 155, 155, 255);
	mp_set_render_draw_color(renderer, 155, 155, 155, 255);
	mp_render_clear(renderer);

	mp_render_clear(renderer);

	// draw_tile_map(renderer);

	// Pixels
	int tile_w = 32;
	int tile_h = 32;

	// Have everything move around the game_data.camera / player
	// The player is just an offset from the game_data.camera. Camera is always 0,0

	// Only render tiles in the view of the player
	// Currently in world space
	for (int r = 0; r < game_data.camera.w/ tile_w + 1; r++) {
		for (int c = 0; c < game_data.camera.h / tile_h + 1; c++) {
			MP_Rect current_rect = {};
			current_rect.w = tile_w;
			current_rect.h = tile_h;

			current_rect.x = (tile_w * r) - game_data.camera.x;
			current_rect.y = (tile_h * c) - game_data.camera.y;

			mp_set_render_draw_color(renderer, C_Green);

			mp_render_draw_rect(renderer, &current_rect);
		}
	}

	// Create a render scene function that has the follow:
	// 1) Render player 
	// 2) Render entities

	// Camera relative to the game_data.player
	game_data.camera.x = (int)((float)game_data.player.position_ws.x - (float)game_data.camera.w / 2.0f);
	game_data.camera.y = (int)((float)game_data.player.position_ws.y - (float)game_data.camera.h / 2.0f);

	game_data.player.position_cs.x = (float)game_data.camera.w / 2.0f - game_data.player.w / 2;
	game_data.player.position_cs.y = (float)game_data.camera.h / 2.0f - game_data.player.h / 2;
	mp_set_texture_alpha_mod(game_data.player.image->texture, 100);

	MP_Rect player_rect = {(int)game_data.player.position_cs.x, (int)game_data.player.position_cs.y, game_data.player.w, game_data.player.h};
	mp_render_copy(renderer, game_data.player.image->texture, NULL, &player_rect);

	Font* font = get_font(game_data.selected_font);
	draw_mp_library_debug_images(renderer, *font, game_data.player.image->texture, toggle_debug_images);
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
	game_data.player = create_player(get_image("player"), player_speed);

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
			toggle_debug_images = !toggle_debug_images;
		}

		// update()

		// render
		render(renderer, game_data);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}