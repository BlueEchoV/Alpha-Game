#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static inline int64_t GetTicks()
{
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks)) {
		log("Error: QueryPerformanceCounter failed.");
		assert(false);
    }
    return ticks.QuadPart;
}

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
	game_data.player = create_player(get_image(IT_Player_Rugged_Male), player_speed);

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	game_data.camera.w = renderer->window_width;
	game_data.camera.h = renderer->window_height;
	game_data.player.w = 96;
	game_data.player.h = 96;

	V2 test_vec1 = { 20, 20 };
	V2 test_vec2 = { 10, 10 };

	V2 result_vec = test_vec1 - test_vec2;

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
		float player_x_delta = 0.0f;
		float player_y_delta = 0.0f;
		if (key_pressed_and_held(KEY_W)) {
			player_y_delta = 1.0f;
		}
		if (key_pressed_and_held(KEY_S)) {
			player_y_delta = -1.0f;
		}
		if (key_pressed_and_held(KEY_D)) {
			player_x_delta = 1.0f;
		}
		if (key_pressed_and_held(KEY_A)) {
			player_x_delta = -1.0f;
		}

		uint64_t temp = GetTicks();
		REF(temp);

		player_x_delta *= game_data.player.speed;
		player_y_delta *= game_data.player.speed;

		if (player_x_delta != 0.0f && player_y_delta != 0.0f) {
			// See HMH 042 at 24:30 for reference. 
			// a^2 + b^2 = c^2
			// v^2 + v^2 = d^2 to d sqrt(1/2)
			// player_x_delta represents the d and sqrt of 1/2 is the hard coded value
			player_x_delta *= 0.707106781187f;
			player_y_delta *= 0.707106781187f;
		}

		game_data.player.position_ws.x += player_x_delta;
		game_data.player.position_ws.y += player_y_delta;

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