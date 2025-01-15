#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	MP_Renderer* renderer = mp_create_renderer(hInstance);
	load_images(renderer);
	load_fonts(renderer);

	Game_Data game_data = {};
	game_data.selected_font = FT_Basic;

	int player_speed = 1;
	game_data.player = create_player(get_image(IT_Top_Down_Player_Demo), player_speed);

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	game_data.camera.w = renderer->window_width;
	game_data.camera.h = renderer->window_height;
	game_data.player.w = 200;
	game_data.player.h = 200;

	V2_F test_vec1 = { 20, 20 };
	V2_F test_vec2 = { 10, 10 };

	V2_F result_vec = test_vec1 - test_vec2;

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
			game_data.player.position_ws.y += game_data.player.speed;
		}
		if (key_pressed_and_held(KEY_S)) {
			game_data.player.position_ws.y -= game_data.player.speed;
		}
		if (key_pressed_and_held(KEY_D)) {
			game_data.player.position_ws.x += game_data.player.speed;
		}
		if (key_pressed_and_held(KEY_A)) {
			game_data.player.position_ws.x -= game_data.player.speed;
		}
		if (key_pressed_and_held(VK_SHIFT)) {
			game_data.player.speed = player_speed * 2;
		} else {
			game_data.player.speed = player_speed;
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