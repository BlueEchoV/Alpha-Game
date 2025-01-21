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

	// Per second
	int player_speed = 100;
	game_data.player = create_player(get_image(IT_Player_Rugged_Male), player_speed);

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	game_data.camera.w = renderer->window_width;
	game_data.camera.h = renderer->window_height;
	game_data.player.w = 96;
	game_data.player.h = 96;

	bool running = true;

	uint64_t current_frame_time = 0;
	uint64_t last_frame_time = 0;
	float delta_time = 0;
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

		current_frame_time = mp_get_ticks_64();
		// Convert to seconds
		delta_time = (float)(current_frame_time - last_frame_time) / 1000;
		last_frame_time = current_frame_time;
		// log("Current Time %i", current_frame_time);
		// uint64_t temp = uint64_t(delta_time * 1000.0f);
		// log("Frame Time milliseconds %i", temp);

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
		if (key_pressed_and_held(VK_SPACE)) {
			Player* p = &game_data.player;
			V2 arrow_pos = {(float)p->pos.x - (p->w / 2), (float)p->pos.y - (p->h / 2)};
			log("Spawning arrow at x = %f, y = %f", arrow_pos.x, arrow_pos.y);
			fire_arrow(
				game_data, 
				IT_Arrow_1, 
				10, 
				{ arrow_pos.x + 500, arrow_pos.y },
				arrow_pos	
			);
		}

		player_x_delta *= game_data.player.speed * delta_time;
		player_y_delta *= game_data.player.speed * delta_time;

		if (player_x_delta != 0.0f && player_y_delta != 0.0f) {
			// See HMH 042 at 24:30 for reference. 
			// a^2 + b^2 = c^2
			// v^2 + v^2 = d^2 to d sqrt(1/2)
			// player_x_delta represents the d and sqrt of 1/2 is the hard coded value
			player_x_delta *= 0.707106781187f;
			player_y_delta *= 0.707106781187f;
		}

		game_data.player.pos.x += player_x_delta;
		game_data.player.pos.y += player_y_delta;

		if (key_pressed_and_held(VK_SHIFT)) {
			game_data.player.speed = player_speed * 2;
		} else {
			game_data.player.speed = player_speed;
		}

		if (key_pressed(VK_OEM_3)) {
			Globals::toggle_debug_images = !Globals::toggle_debug_images;
		}

		// Update
		for (Arrow& arrow : game_data.arrows) {
			update_arrow(arrow, delta_time);
		}
		// Render
		render(renderer, game_data);
	}

	ReleaseDC(renderer->open_gl.window_handle, renderer->open_gl.window_dc);
	return 0;
}