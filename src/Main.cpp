#include "GL_Functions.h"
#include "Game.h"
#include "Image.h"
#include "Audio_xAudio2.h"
#include "Audio_DirectSound.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math.h>

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
	load_images();
	load_fonts();

	game_data.selected_font = FT_Basic;

	int player_speed = 100;
	game_data.player = create_player(get_image(IT_Player_Rugged_Male), { 0,0 }, player_speed);
	game_data.camera = create_camera(game_data.player);

	bool running = true;

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
	global_secondary_buffer->Play(0, 0, DSBPLAY_LOOPING);

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
			if (game_data.player.can_fire) {
				V2 mouse_cs_pos = get_mouse_position(Globals::renderer->open_gl.window_handle);
				V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, game_data.camera.pos_ws);
				spawn_projectile(game_data, game_data.player.rb.pos_ws, mouse_ws_pos);
				Globals::debug_total_arrows++;
				game_data.player.can_fire = false;
			}
		}

		if (game_data.player.fire_cooldown <= 0) {
			game_data.player.can_fire = true;
			game_data.player.fire_cooldown = (float)game_data.player.fire_rate;
		} 
		if (game_data.player.can_fire == false) {
			game_data.player.fire_cooldown -= delta_time;
		}

		if (key_pressed_and_held(KEY_Q)) {
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
				if (enemy_unit != NULL) {
					enemy_unit->destroyed = true;
				}
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
			Globals::debug_show_stats = !Globals::debug_show_stats;
		}

		// Update
		for (Handle& projectile_handle : game_data.projectile_handles) {
			Projectile* p = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handle);
			if (p != NULL) {
				update_projectile(*p, delta_time);
			}
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
				if (proj == NULL) {
					continue;
				}
				Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
				if (unit == NULL) {
					continue;
				}
				float distance_between = calculate_distance_between(proj->rb.pos_ws, unit->rb.pos_ws);
				float radius_sum = (float)proj->w + (float)unit->w;
				if (distance_between <= radius_sum) {
					proj->destroyed = true;
					unit->destroyed = true;
				}
			}
		}

		// Render
		render(game_data, delta_time);

		delete_destroyed_entities_from_handles(game_data);
	}


	ReleaseDC(Globals::renderer->open_gl.window_handle, Globals::renderer->open_gl.window_dc);
	return 0;
}