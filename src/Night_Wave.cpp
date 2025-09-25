#include "Night_Wave.h"

Night_Wave_Data bad_night_wave_data = {
	// level		total_enemies		spawning_cd
	   1,			20,					0.5f
};

std::unordered_map<std::string, Night_Wave_Data> night_wave_data_map;

Night_Wave_Data* get_night_wave_data(std::string night_wave_name) {
	if (night_wave_data_map.find(night_wave_name) != night_wave_data_map.end()) {
		return &night_wave_data_map[night_wave_name];
	}

	return &bad_night_wave_data;
};

MP_Rect get_spawn_region_in_pixels_ws(Night_Wave& night_wave, Tile_Map& map) {
	MP_Rect result = {};

	int spawn_region_size_in_pixels = night_wave.spawn_region_size_in_tiles * Globals::tile_w;

	switch (night_wave.spawn_direction) {
	case SD_North: {
		result.x = -(map.w_in_pixels / 2) + spawn_region_size_in_pixels;
		result.y = map.h_in_pixels / 2 - spawn_region_size_in_pixels;
		result.w = map.w_in_pixels - spawn_region_size_in_pixels * 2;
		result.h = spawn_region_size_in_pixels;
		break;
	}
	case SD_South: {
		result.x = -(map.w_in_pixels / 2) + spawn_region_size_in_pixels;
		result.y = -(map.h_in_pixels / 2);
		result.w = map.w_in_pixels - spawn_region_size_in_pixels * 2;
		result.h = spawn_region_size_in_pixels;
		break;
	}
	case SD_East: {
		result.x = (map.w_in_pixels / 2) - spawn_region_size_in_pixels;
		result.y = -(map.h_in_pixels / 2) + spawn_region_size_in_pixels;
		result.w = spawn_region_size_in_pixels;
		result.h = map.h_in_pixels - spawn_region_size_in_pixels * 2;
		break;
	}
	case SD_West: {
		result.x = -(map.w_in_pixels / 2) + night_wave.spawn_region_size_in_tiles;
		result.y = -(map.h_in_pixels / 2) + spawn_region_size_in_pixels;
		result.w = spawn_region_size_in_pixels;
		result.h = map.h_in_pixels - spawn_region_size_in_pixels * 2;
		break;
	}
	default: {
		log("Error: Spawn region not specified");
		break;
	}
	}

	return result;
}

int normal_max_waves = 20;
int hard_max_waves = 20;
int endless_max_waves = INT_MAX;
int get_max_waves_from_difficulty(Map_Difficulty md) {
	switch (md) {
	case MD_Normal: {
		return normal_max_waves;
	}
	case MD_Hard: {
		return hard_max_waves;
	}
	case MD_Endless: {
		return endless_max_waves;
	}
	default: {
		log("Difficulty not specified");
		return -1;
	}
	}
}

// Call this at the end of every day?
Night_Wave create_night_wave(Map_Difficulty difficulty, Spawn_Direction spawn_direction, int spawn_region_size_in_tiles, 
	int current_wave, int enemies_to_spawn) {
	Night_Wave result = {};

	Night_Wave_Data* data = get_night_wave_data("");

	result.difficulty = difficulty;
	result.current_wave = current_wave;
	result.spawning_cd = create_cooldown(data->max_spawning_cd);
	result.spawn_direction = spawn_direction;
	result.spawn_region_size_in_tiles = spawn_region_size_in_tiles;
	result.begin_spawning = false;
	result.total_to_spawn = enemies_to_spawn;
	result.total_spawned = 0;

	return result;
}

void reset_and_scale_night_wave(std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage, Night_Wave& nw) {
	// Reset values
	nw.begin_spawning = false;
	nw.total_spawned = 0;
	REF(unit_handles);
	REF(unit_storage);
	/*
	for (Handle& h : unit_handles) {
		Unit* unit = get_unit_from_handle(unit_storage, h);
		unit->destroyed = true;
	}
	*/

	// Scale Wave
	if (nw.current_wave < get_max_waves_from_difficulty(nw.difficulty)) {
		nw.current_wave++;
		// TODO: Exponential now. Need to change this later.
		nw.total_to_spawn += 2;
		// Randomize spawn_direction to a different value
        int current_dir = nw.spawn_direction;
        int new_dir;
        do {
            new_dir = rand() % 4;
        } while (new_dir == current_dir);
        nw.spawn_direction = (Spawn_Direction)new_dir;
	}
}

bool temp = false;
void spawn_and_update_night_wave(std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage,
	Night_Wave& night_wave, int& active_enemy_units, Player& player, Tile_Map& tile_map, float delta_time) {
	if (night_wave.begin_spawning) {
		if (night_wave.total_spawned < night_wave.total_to_spawn) {
			if (trigger_cooldown(night_wave.spawning_cd)) {
				V2 random_pos_ws = {};
				MP_Rect spawn_region_ws = get_spawn_region_in_pixels_ws(night_wave, tile_map);
				random_pos_ws.x = (float)((int)spawn_region_ws.x + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.w));
				random_pos_ws.y = (float)((int)spawn_region_ws.y + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.h));

				std::string unit_name = "ravenous_skulk";
#if 0
				std::string unit_name = "";
				if (temp) {
					unit_name = "ravenous_skulk";

					temp = false;
				}
				else {
					unit_name = "gravebound_peasant";
					temp = true;
				}
#endif

				spawn_unit(
					F_Enemies,
					unit_name,
					AS_Walking,
					APS_Fast,
					AM_Animate_Looping,
					unit_storage,
					unit_handles,
					&player,
					random_pos_ws
				);
				night_wave.total_spawned++;
				active_enemy_units++;
			}
		}
		else {
			if (active_enemy_units <= 0) {
				reset_and_scale_night_wave(unit_handles, unit_storage, night_wave);
			}
		}
	} 
	update_cooldown(night_wave.spawning_cd, delta_time);
}

// NOTE: Draw diagnal lines? Or dotted lines?
void draw_night_wave_spawn_region(Color_Type c, Night_Wave& night_wave, Tile_Map& tile_map, V2 camera_pos) {
	MP_Rect spawn_region_ws = get_spawn_region_in_pixels_ws(night_wave, tile_map);
	V2 pos_cs = convert_ws_to_cs({(float)spawn_region_ws.x, (float)spawn_region_ws.y}, camera_pos);
	MP_Rect spawn_region_cs = { (int)pos_cs.x, (int)pos_cs.y, spawn_region_ws.w, spawn_region_ws.h };

	mp_set_render_draw_color(c);
	mp_render_draw_rect(&spawn_region_cs);
}

/*
Type_Descriptor night_wave_data_type_descriptors[] = {
	FIELD(Night_Wave_Data, VT_Int, level),
	FIELD(Night_Wave_Data, VT_Int, total_to_spawn),
	FIELD(Night_Wave_Data, VT_Float, max_spawning_cd)
};

void load_night_wave_data_csv(CSV_Data* data) {
	std::vector<Night_Wave> night_wave_data;
	night_wave_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_night_wave_data_type_descriptors(night_wave_data_type_descriptors);

	load_csv_data_file(data, (char*)night_wave_data.data(), safe_night_wave_data_type_descriptors, sizeof(Night_Wave));

	for (Night_Wave_Data& night_wave_data_iterator : night_wave_data_map) {
		night_wave_data_map[night_wave_data_iterator.night_wave_name] = night_wave_data_iterator;
	}
}
*/
