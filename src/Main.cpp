#include "GL_Functions.h"
#include "Sprite_Sheet.h"
#include "Tile_Map.h"
#include "Audio_xAudio2.h"
#include "Audio_DirectSound.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math.h>

#include "Game_Data.h"
#include "Debug.h"

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
#define Pi32 3.14159265359f

Game_Data game_data = {};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	Globals::renderer = mp_create_renderer(hInstance);

	// Just raw images
	load_images();

	load_fonts();

	CSV_Data unit_data_csv = create_open_csv_data("data\\unit_data.csv");
	open_csv_data_file(&unit_data_csv);
	load_unit_data_csv(&unit_data_csv);
	close_csv_data_file(&unit_data_csv);

	CSV_Data sprite_sheet_data_csv = create_open_csv_data("data\\sprite_sheet_data.csv");
	open_csv_data_file(&sprite_sheet_data_csv);
	load_sprite_sheet_data_csv(&sprite_sheet_data_csv);
	close_csv_data_file(&sprite_sheet_data_csv);
	// Sprite sheets for the animation trackers
	load_sprite_sheets();

	CSV_Data weapon_data_csv = create_open_csv_data("data\\weapon_data.csv");
	open_csv_data_file(&weapon_data_csv);
	load_weapon_data_csv(&weapon_data_csv);
	close_csv_data_file(&weapon_data_csv);

	CSV_Data projectile_data_csv = create_open_csv_data("data\\projectile_data.csv");
	open_csv_data_file(&projectile_data_csv);
	load_projectile_data_csv(&projectile_data_csv);
	close_csv_data_file(&projectile_data_csv);

	game_data.selected_font = FT_Basic;

	game_data.player = create_player("player_1", { 0.0f, 0.0f });
	game_data.player.health_bar.current_hp -= 50;
	game_data.camera = create_camera(game_data.player.rb.pos_ws);

	game_data.current_horde = create_horde(F_Enemies, HT_Not_Specified, SD_North, 2);

	// This is like the "frames per second" in a video or the "resolution" of your sound timeline. 
	//		It’s how many "pixels" (samples) you capture per second to draw the sound.
    int samples_per_second = 48000;
	// This is like the "color depth" of each pixel—how much data each sound pixel holds. Here, 
	//		it’s 4 bytes total (2 bytes per uint16_t × 2 channels).
    int bytes_per_sample = sizeof(int16_t) * 2;
	// This is the "canvas size" or total number of pixels in your sound image. It’s like saying 
	//		your video frame buffer holds 2 seconds of pixels at 48,000 pixels per second.
    int secondary_buffer_size = 2 * samples_per_second * bytes_per_sample;
	// This is like the "pattern frequency" in your image—how often a repeating design (like a stripe) 
	//		appears. In sound, it’s the pitch of the note.
	int tone_hz = 256;
	// This is the "width" of one repeating pattern (one cycle) in your sound image, measured in pixels (samples).
	// int square_wave_period = samples_per_second / tone_hz;
	// This is like the "brightness" or "intensity" of your pixel color—how vivid the sound pixel is.
	int16_t tone_volume = 3000;
	uint32_t running_sample_index = 0;
	int wave_period = samples_per_second / tone_hz;
    
	bool sound_is_playing = false;
	init_direct_sound(&Globals::renderer->open_gl.window_handle, samples_per_second, secondary_buffer_size);
	// UNCOMMENT THIS PLAY BUFFER
	// global_secondary_buffer->Play(0, 0, DSBPLAY_LOOPING);

	// NOTE: Direct Sound output test
	DWORD play_cursor;
	DWORD write_cursor;
	if (SUCCEEDED(global_secondary_buffer->GetCurrentPosition(&play_cursor, &write_cursor))) {
		DWORD byte_to_lock = (running_sample_index * bytes_per_sample) % secondary_buffer_size;
		DWORD bytes_to_write;
		// TODO: We need a more accurate check than ByteToLock == PlayCursor
		if (byte_to_lock == play_cursor)
		{
			// Play cursor is at the same spot
			bytes_to_write = secondary_buffer_size;
		}
		else if(byte_to_lock > play_cursor)
		{
			// Play cursor is behind
			bytes_to_write = secondary_buffer_size - byte_to_lock; // region 1
			bytes_to_write += play_cursor; // region 2
		}
		else
		{
			// Play cursor is in front
			bytes_to_write = play_cursor - byte_to_lock; // region 1
		}

		VOID* region_1;
		DWORD region_1_size;
		VOID* region_2;
		DWORD region_2_size;
		if (SUCCEEDED(global_secondary_buffer->Lock(byte_to_lock, bytes_to_write,
			&region_1, &region_1_size, &region_2, &region_2_size, 0))) {
			// All good, we can write to the buffer
			// TODO: assert that region_1_size/region_2_size are valid 
			int16_t* sample_out = (int16_t*)region_1;
			DWORD region_1_sample_count = region_1_size / bytes_per_sample;
			for (DWORD sample_index = 0; sample_index < region_1_sample_count; ++sample_index) {
				f32 t = 2.0f * Pi32 * (f32)running_sample_index / (f32)wave_period;
				f32 sine_value = sinf(t);
				s16 sample_value = (s16)(sine_value * tone_volume);
				// int16_t sample_value = ((running_sample_index++ / (int32_t)half_square_wave_period) % 2) ? tone_volume: -tone_volume; 
				// Left
				*sample_out++ = sample_value;
				// Right
				*sample_out++ = sample_value;
				++running_sample_index;
			}
			sample_out = (int16_t*)region_2;
			DWORD region_2_sample_count = region_2_size / bytes_per_sample;
			for (DWORD sample_index = 0; sample_index < region_2_sample_count; ++sample_index) {
				f32 t = 2.0f * Pi32 * (f32)running_sample_index / (f32)wave_period;
				f32 sine_value = sinf(t);
				s16 sample_value = (s16)(sine_value * tone_volume);
				// int16_t sample_value = ((running_sample_index++ / (int32_t)half_square_wave_period) % 2) ? tone_volume: -tone_volume; 
				// Left
				*sample_out++ = sample_value;
				// Right
				*sample_out++ = sample_value;
				++running_sample_index;
			}
			global_secondary_buffer->Unlock(region_1, region_1_size, region_2, region_2_size);
		}

	}

	if (!sound_is_playing) {
		global_secondary_buffer->Play(0, 0, DSBPLAY_LOOPING);
		sound_is_playing;
	}
	global_secondary_buffer->Stop();

	uint64_t current_frame_time = 0;
	uint64_t last_frame_time = 0;
	float delta_time = 0;

	float debug_spawning_delay = 0.25f;
	float current_debug_spawning_delay = 0.0f;

	Tile_Map demo_tile_map = create_tile_map(64, 64);

	while (Globals::running) {
		reset_is_pressed();

		MSG message;
		// GetMessage function inside our WinMain will sit and wait for new 
		// messages forever if there aren't any.
		// The new parameter at the end is wRemoveMsg, which tells PeekMessage 
		// what to do with the message peeked. We want to remove the message 
		// from the queue, so we pass PM_REMOVE.
		while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) {
				Globals::running = false;
			}
		}

		current_frame_time = mp_get_ticks_64();
		// Convert to seconds
		delta_time = (float)(current_frame_time - last_frame_time) / 1000;
		last_frame_time = current_frame_time;
		// log("Current Time %i", current_frame_time);
		// uint64_t temp = uint64_t(delta_time * 1000.0f);
		// log("Frame Time milliseconds %i", temp);

		Player* player = &game_data.player;

		// input()
		float player_x_delta = 0.0f;
		float player_y_delta = 0.0f;

		if (!player->dead) {
			if (key_pressed_and_held(KEY_W) && key_pressed_and_held(VK_SHIFT)) {
				change_animation(&player->at, player->at.entity_name, AS_Running, player->at.fd, APS_Speed_Based);
				player_y_delta = 1.0f;
			}
			if (key_pressed_and_held(KEY_W)) {
				change_animation(&player->at, player->at.entity_name, AS_Walking, player->at.fd, APS_Speed_Based);
				player_y_delta = 1.0f;
			}

			if (key_pressed_and_held(KEY_S) && key_pressed_and_held(VK_SHIFT)) {
				change_animation(&player->at, player->at.entity_name, AS_Running, player->at.fd, APS_Speed_Based);
				player_y_delta = -1.0f;
			}
			if (key_pressed_and_held(KEY_S)) {
				change_animation(&player->at, player->at.entity_name, AS_Walking, player->at.fd, APS_Speed_Based);
				player_y_delta = -1.0f;
			}

			if (key_pressed_and_held(KEY_A) && key_pressed_and_held(VK_SHIFT)) {
				player->at.fd = FD_W;
				change_animation(&player->at, player->at.entity_name, AS_Running, player->at.fd, APS_Speed_Based);
				player_x_delta = -2.0f;
			}
			else if (key_pressed_and_held(KEY_A)) {
				player->at.fd = FD_W;
				change_animation(&player->at, player->at.entity_name, AS_Walking, player->at.fd, APS_Speed_Based);
				player_x_delta = -1.0f;
			}

			if (key_pressed_and_held(KEY_D) && key_pressed_and_held(VK_SHIFT)) {
				player->at.fd = FD_E;
				change_animation(&player->at, player->at.entity_name, AS_Running, player->at.fd, APS_Speed_Based);
				player_x_delta = 2.0f;
			}
			else if (key_pressed_and_held(KEY_D)) {
				player->at.fd = FD_E;
				change_animation(&player->at, player->at.entity_name, AS_Walking, player->at.fd, APS_Speed_Based);
				player_x_delta = 1.0f;
			}

			if (!key_pressed_and_held(KEY_A) && !key_pressed_and_held(KEY_D) &&
				!key_pressed_and_held(KEY_W) && !key_pressed_and_held(KEY_S)) {
				change_animation(&player->at, player->at.entity_name, AS_Idle, player->at.fd, APS_Slow);
			}

			if (player->weapon == nullptr) {
				equip_weapon(player->weapon, "bow");
			}

			if (key_pressed(KEY_1)) {
				equip_weapon(player->weapon, "bow");
			}
			if (key_pressed(KEY_2)) {
				equip_weapon(player->weapon, "pistol");
			}

			if (key_pressed_and_held(VK_SPACE)) {
				player->weapon->fire_weapon(game_data.projectile_handles, game_data.projectile_storage,
					game_data.camera, game_data.player.rb.pos_ws, F_Player);
			}

			player->weapon->update_weapon(delta_time);
			update_animation_tracker(&player->at, delta_time, (float)player->rb.current_speed);
		}

		if (key_pressed(KEY_3)) {
			player->health_bar.current_hp -= 5;
		}
		if (key_pressed(KEY_4)) {
			player->health_bar.current_hp += 5;
		}

		if (current_debug_spawning_delay <= 0.0f) {
			current_debug_spawning_delay = debug_spawning_delay;
			if (key_pressed_and_held(KEY_Q)) {
				V2 mouse_position = get_mouse_position(Globals::renderer->open_gl.window_handle);
				mouse_position = convert_cs_to_ws(mouse_position, game_data.camera.pos_ws);
				spawn_unit(
					F_Enemies,
					"hellhound",
					AS_Walking,
					game_data.unit_storage,
					game_data.enemy_unit_handles,
					&game_data.player,
					mouse_position
				);
			}
		}
		else {
			current_debug_spawning_delay -= delta_time;
		}

		if (key_pressed(KEY_R)) {
			game_data.current_horde.begin_spawning = true;
		}
		if (key_pressed(KEY_T)) {
			game_data.current_horde = create_horde(F_Enemies, HT_Not_Specified, SD_West, 2);
		}

		if (key_pressed(KEY_E)) {
			for (Handle handle : game_data.enemy_unit_handles) {
				Unit* enemy_unit = get_entity_pointer_from_handle(game_data.unit_storage, handle);
				if (enemy_unit != NULL) {
					enemy_unit->destroyed = true;
				}
			}
		}

		player_x_delta *= game_data.player.rb.current_speed * delta_time;
		player_y_delta *= game_data.player.rb.current_speed * delta_time;

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

		// Collision with the map
		// TODO: Add these to the tilemap struct?
		// TODO: DONT FORGET TO ACCOUNT FOR THE APPROPRIATE RANGE (-32 to 31)
		V2* p_pos = &game_data.player.rb.pos_ws;
		if (p_pos->x < (float)demo_tile_map.left_ws) {
			p_pos->x = (float)demo_tile_map.left_ws;
		}
		if (p_pos->x > (float)demo_tile_map.right_ws) {
			p_pos->x = (float)demo_tile_map.right_ws;
		}
		if (p_pos->y < (float)demo_tile_map.bottom_ws) {
			p_pos->y = (float)demo_tile_map.bottom_ws;
		}
		if (p_pos->y > (float)demo_tile_map.top_ws) {
			p_pos->y = (float)demo_tile_map.top_ws;
		}

		if (key_pressed_and_held(VK_SHIFT)) {
			game_data.player.rb.current_speed = game_data.player.rb.base_speed * 2;
		} else {
			game_data.player.rb.current_speed = game_data.player.rb.base_speed;
		}

		// NOTE: Bind these all to console in the future?
		if (key_pressed(VK_F1)) {
			Globals::toggle_debug_images = !Globals::toggle_debug_images;
		}
		if (key_pressed(VK_F2)) {
			Globals::debug_show_coordinates = !Globals::debug_show_coordinates;
		}
		if (key_pressed(VK_F3)) {
			Globals::debug_show_stats = !Globals::debug_show_stats;
		} 
		if (key_pressed(VK_F4)) {
			Globals::debug_show_colliders = !Globals::debug_show_colliders;
		}
		if (key_pressed(VK_F5)) {
			Globals::toggle_debug_images = !Globals::toggle_debug_images;
			if (Globals::debug_show_coordinates != Globals::toggle_debug_images) {
				Globals::debug_show_coordinates = !Globals::debug_show_coordinates;
			} 
			if (Globals::debug_show_stats != Globals::toggle_debug_images) {
				Globals::debug_show_stats = !Globals::debug_show_stats;
			}
			if (Globals::debug_show_colliders != Globals::toggle_debug_images) {
				Globals::debug_show_colliders = !Globals::debug_show_colliders;
			}
		}

		// Update
		for (Handle& projectile_handle : game_data.projectile_handles) {
			Projectile* p = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handle);
			if (p != NULL) {
				if (!player->dead) {
					update_projectile(*p, delta_time);
				}
			}
		}
		for (Handle enemy_unit_handle : game_data.enemy_unit_handles) {
			Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
			if (unit != NULL) {
				if (!player->dead) {
					update_unit(*unit, delta_time);
				}
				update_animation_tracker(&unit->at, delta_time, (float)unit->rb.current_speed);
			}
		}

		if (!game_data.player.dead) {
			// Collision
			// TODO: Add the destroyed checks into a function for consistency
			for (Handle& projectile_handle : game_data.projectile_handles) {
				for (Handle enemy_unit_handle : game_data.enemy_unit_handles) {
					Projectile* proj = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handle);
					if (proj == NULL || proj->destroyed == true) {
						continue;
					}
					Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
					if (unit == NULL || unit->dead == true) {
						continue;
					}
					if (check_rb_collision(&proj->rb, &unit->rb)) {
						proj->destroyed = true;

						unit->health_bar.current_hp -= proj->damage;
						if (unit->health_bar.current_hp <= 0) {
							unit->dead = true;
						}
					}
				}
			}

			for (Handle& enemy_handle : game_data.enemy_unit_handles) {
				Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_handle);
				if (unit == NULL || unit->dead == true) {
					continue;
				}
				if (unit->can_attack) {
					if (check_rb_collision(&game_data.player.rb, &unit->rb)) {
						player->health_bar.current_hp -= unit->damage;
						if (player->health_bar.current_hp <= 0) {
							player->dead = true;
						}
					}
				}
			}

			spawn_and_update_horde(game_data.enemy_unit_handles, game_data.unit_storage, game_data.current_horde, game_data.player,
				demo_tile_map, delta_time);
		}

		// Render

		// TODO: Move everything into here at some point
		// render(game_data, delta_time);
		MP_Renderer* renderer = Globals::renderer;

		int window_width = 0;
		int window_height = 0;
		get_window_size(renderer->open_gl.window_handle, window_width, window_height);
		MP_Rect viewport = { 0, 0, window_width, window_height};
		mp_render_set_viewport(&viewport);

		renderer->window_width= window_width;
		renderer->window_height = window_height;

		update_camera(game_data.camera, game_data.player.rb.pos_ws);

		mp_set_render_draw_color(155, 155, 155, 255);

		mp_render_clear();

		Camera camera = game_data.camera;

		draw_entire_map(camera, demo_tile_map);

		draw_horde_spawn_region(CT_Red, game_data.current_horde, demo_tile_map, game_data.camera.pos_ws);

		// draw_player(game_data.player, game_data.camera.pos_ws);
		// draw_colliders(&game_data.player.rb, game_data.camera.pos_ws);

		Font* font = get_font(game_data.selected_font);
		debug_draw_all_debug_info(game_data, *font, get_image("dummy_image")->texture, delta_time);

		for (Handle projectile: game_data.projectile_handles) {
			Projectile* p = get_entity_pointer_from_handle(game_data.projectile_storage, projectile);
			if (p == NULL) {
				log("Error: handle returned null");
				continue;
			}
			draw_projectile((int)game_data.camera.pos_ws.x, (int)game_data.camera.pos_ws.y, *p);
			if (Globals::debug_show_coordinates) {
				debug_draw_coor(game_data, p->rb.pos_ws, false, p->rb.pos_ws, true, 
					CT_Green, true, "WS Pos: ");
			}
			if (Globals::debug_show_colliders) {
				draw_colliders(&p->rb, game_data.camera.pos_ws);
			}
		}

		for (Handle zombie_handle: game_data.enemy_unit_handles) {
			Unit* u = get_entity_pointer_from_handle(game_data.unit_storage, zombie_handle);
			if (u == NULL) {
				log("Error: handle returned null");
				continue;
			}
			if (u->dead) {
				change_animation(&u->at, u->unit_name, AS_Death, u->at.fd, u->at.aps);
			}

			draw_unit(*u, game_data.camera.pos_ws);

			if (Globals::debug_show_colliders && u->dead == false) {
				draw_colliders(&u->rb, game_data.camera.pos_ws);
			}
		}

		draw_player(game_data.player, game_data.camera.pos_ws);

		if (game_data.player.dead) {
			draw_string(*font, "Game Over", CT_Red_Wine, true, Globals::resolution_x / 2,  Globals::resolution_y / 2, 5, true);
		}

		mp_render_present();

		delete_destroyed_entities_from_game_data_handles(game_data);
	}


	destroy_game_data(game_data);
	ReleaseDC(Globals::renderer->open_gl.window_handle, Globals::renderer->open_gl.window_dc);
	return 0;
}