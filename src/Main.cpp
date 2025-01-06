#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	MP_Renderer* renderer = mp_create_renderer(hInstance);

	Game_Data game_data = {};

	Image sun = load_image(renderer, "assets\\sun.png");

	int player_speed = 1;
	game_data.player = create_player(&sun, player_speed);

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	MP_Rect camera = {0, 0};
	get_window_size(renderer->open_gl.window_handle, camera.w, camera.h);
	Player player = {};
	player.w = 200;
	player.h = 200;

	Font font_1 = load_font(renderer, "assets\\basic_font.png");

	bool toggle_debug_images = true;
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

		if (key_pressed_and_held(KEY_W)) {
			player.position_ws.y += player_speed;
		}
		if (key_pressed_and_held(KEY_S)) {
			player.position_ws.y -= player_speed;
		}
		if (key_pressed_and_held(KEY_D)) {
			player.position_ws.x += player_speed;
		}
		if (key_pressed_and_held(KEY_A)) {
			player.position_ws.x -= player_speed;
		}
		if (key_pressed(VK_OEM_3)) {
			toggle_debug_images = !toggle_debug_images;
		}

		// Camera relative to the player
		camera.x = (int)((float)player.position_ws.x - (float)camera.w / 2.0f);
		camera.y = (int)((float)player.position_ws.y - (float)camera.h / 2.0f);

		mp_set_render_draw_color(renderer, 155, 155, 155, 255);
		mp_render_clear(renderer);

		// draw_tile_map(renderer);

		// Pixels
		int tile_w = 32;
		int tile_h = 32;

		// Have everything move around the camera / player
		// The player is just an offset from the camera. Camera is always 0,0

		// Only render tiles in the view of the player
		// Currently in world space
		for (int r = 0; r < camera.w/ tile_w + 1; r++) {
			for (int c = 0; c < camera.h / tile_h + 1; c++) {
				MP_Rect current_rect = {};
				current_rect.w = tile_w;
				current_rect.h = tile_h;

				current_rect.x = (tile_w * r) - camera.x;
				current_rect.y = (tile_h * c) - camera.y;

				mp_set_render_draw_color(renderer, C_Green);

				mp_render_draw_rect(renderer, &current_rect);
			}
		}

		// Create a render scene function that has the follow:
		// 1) Render player 
		// 2) Render entities
		player.position_cs.x = (float)camera.w / 2.0f - player.w / 2;
		player.position_cs.y = (float)camera.h / 2.0f - player.h / 2;
		mp_set_texture_alpha_mod(sun.texture, 100);

		MP_Rect player_rect = {(int)player.position_cs.x, (int)player.position_cs.y, player.w, player.h};
		mp_render_copy(renderer, sun.texture, NULL, &player_rect);

		draw_mp_library_debug_images(renderer, font_1, sun.texture, toggle_debug_images);
		mp_render_present(renderer);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}