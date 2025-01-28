#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	Globals::renderer = mp_create_renderer(hInstance);
	load_images();
	load_fonts();

	Game_Data game_data = {};
	game_data.selected_font = FT_Basic;

	int player_speed = 100;
	game_data.player = create_player(get_image(IT_Player_Rugged_Male), { 0,0 }, player_speed);
	game_data.camera = create_camera(game_data.player);

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

		if (key_pressed(VK_SPACE)) {
			fire_player_arrow(
				game_data, 
				IT_Arrow_1, 
				32, 
				32,
				100
			);
		}
		if (key_pressed(KEY_Q)) {
			Zombie zombie = spawn_zombie(
				get_image(IT_Enemy_Clothed_Zombie),
				&game_data.player,
				{ 0, 0 },
				150, 
				150, 
				10, 
				100, 
				10
			);
			game_data.zombies.push_back(zombie);
			log("Spawning Zombie");
		}

		player_x_delta *= game_data.player.rb.speed * delta_time;
		player_y_delta *= game_data.player.rb.speed * delta_time;

		if (player_x_delta != 0.0f && player_y_delta != 0.0f) {
			// See HMH 042 at 24:30 for reference. 
			// a^2 + b^2 = c^2
			// v^2 + v^2 = d^2 to d sqrt(1/2)
			// player_x_delta represents the d and sqrt of 1/2 is the hard coded value
			player_x_delta *= 0.707106781187f;
			player_y_delta *= 0.707106781187f;
		}

		game_data.player.rb.pos_ws.x += player_x_delta;
		game_data.player.rb.pos_ws.y += player_y_delta;

		if (key_pressed_and_held(VK_SHIFT)) {
			game_data.player.rb.speed = player_speed * 2;
		} else {
			game_data.player.rb.speed = player_speed;
		}

		if (key_pressed(VK_F1)) {
			Globals::toggle_debug_images = !Globals::toggle_debug_images;
		}
		if (key_pressed(VK_F2)) {
			Globals::debug_show_coordinates = !Globals::debug_show_coordinates;
		}

		// Update
		for (Arrow& arrow : game_data.arrows) {
			update_arrow(arrow, delta_time);
		}
		for (Zombie& zombie : game_data.zombies) {
			update_zombie(zombie, delta_time);
		}
		// Render
		render(game_data);
	}

	ReleaseDC(Globals::renderer->open_gl.window_handle, Globals::renderer->open_gl.window_dc);
	return 0;
}