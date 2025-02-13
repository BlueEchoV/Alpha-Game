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
			V2 mouse_cs_pos = get_mouse_position(Globals::renderer->open_gl.window_handle);
			V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, game_data.camera.pos_ws);
			spawn_projectile(game_data, game_data.player.rb.pos_ws, mouse_ws_pos);
			Globals::debug_total_arrows++;
		}
		if (key_pressed(KEY_Q)) {
			V2 mouse_position = get_mouse_position(Globals::renderer->open_gl.window_handle);
			mouse_position = convert_cs_to_ws(mouse_position, game_data.camera.pos_ws);
			spawn_unit(
				UT_Zombie,
				game_data.unit_storage,
				game_data.enemy_unit_handles,
				&game_data.player,
				mouse_position
			);
			log("Spawning Zombie");
		}
		if (key_pressed(KEY_E)) {
			for (Handle handle : game_data.enemy_unit_handles) {
				Unit* enemy_unit = get_entity_pointer_from_handle(game_data.unit_storage, handle);
				enemy_unit->destroyed = true;
			}
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
		if (key_pressed(VK_F3)) {
			Globals::debug_show_stats= !Globals::debug_show_stats;
		}

		// Update
		for (Handle& projectile_handle : game_data.projectile_handles) {
			Projectile* p = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handle);
			update_projectile(*p, delta_time);
		}
		for (Handle enemy_unit_handle : game_data.enemy_unit_handles) {
			Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
			if (unit != NULL) {
				update_unit(*unit, delta_time);
			}
		}

		// Collision
		for (Handle& projectile_handle : game_data.projectile_handles) {
			for (Handle enemy_unit_handle : game_data.enemy_unit_handles) {
				Projectile* proj = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handle);
				Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
				float distance_between = calculate_distance_between(proj->rb.pos_ws, unit->rb.pos_ws);
				float radius_sum = (float)proj->w + (float)unit->w;
				if (distance_between <= radius_sum) {
					proj->destroyed = true;
					unit->destroyed = true;
				}
				// Calculate the length from the arrow and the target
			}
		}

		// Render
		render(game_data, delta_time);

		delete_destroyed_entities_from_handles(game_data);
	}


	ReleaseDC(Globals::renderer->open_gl.window_handle, Globals::renderer->open_gl.window_dc);
	return 0;
}