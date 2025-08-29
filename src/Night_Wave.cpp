#include "Night_Wave.h"

Night_Wave_Data bad_night_wave_data = {
	// level		total_enemies		spawning_cd
	   1,			20,					0.5f
};

std::unordered_map<std::string, Night_Wave_Data> night_wave_data;

Night_Wave_Data get_night_wave_data(Night_Wave_Type night_wave_type) {
	Night_Wave_Data result = {};

	if (night_wave_type == NWT_Not_Specified) {
		return bad_night_wave_data;
	}
	return bad_night_wave_data;
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

// Call this at the end of every day?
Night_Wave create_night_wave(Faction faction, Night_Wave_Type nwt, Spawn_Direction spawn_direction, int spawn_region_size_in_tiles) {
	Night_Wave result = {};

	Night_Wave_Data data = get_night_wave_data(nwt);

	result.faction = faction;
	result.level = data.level;
	result.spawning_cd = create_cooldown(data.max_spawning_cd);
	result.spawn_direction = spawn_direction;
	result.spawn_region_size_in_tiles = spawn_region_size_in_tiles;
	result.begin_spawning = false;
	result.total_to_spawn = data.total_to_spawn;
	result.total_spawned = 0;

	return result;
}

void spawn_and_update_night_wave(const std::string& unit_name, std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage,
	Night_Wave& night_wave, Player& player, Tile_Map& tile_map, float delta_time) {
	if (night_wave.begin_spawning) {
		if (night_wave.total_spawned < night_wave.total_to_spawn) {
			if (check_and_update_cooldown(night_wave.spawning_cd, delta_time)) {
				if (night_wave.faction == F_Enemies) {
					V2 random_pos_ws = {};
					MP_Rect spawn_region_ws = get_spawn_region_in_pixels_ws(night_wave, tile_map);
					random_pos_ws.x = (float)((int)spawn_region_ws.x + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.w));
					random_pos_ws.y = (float)((int)spawn_region_ws.y + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.h));
					spawn_unit(
						night_wave.faction,
						unit_name,
						AS_Walking,
						APS_Fast,
						AM_Animate_Looping,
						unit_storage,
						unit_handles,
						&player,
						random_pos_ws
					);
				}
			}
		}
	}
}

// NOTE: Draw diagnal lines? Or dotted lines?
void draw_night_wave_spawn_region(Color_Type c, Night_Wave& night_wave, Tile_Map& tile_map, V2 camera_pos) {
	MP_Rect spawn_region_ws = get_spawn_region_in_pixels_ws(night_wave, tile_map);
	V2 pos_cs = convert_ws_to_cs({(float)spawn_region_ws.x, (float)spawn_region_ws.y}, camera_pos);
	MP_Rect spawn_region_cs = { (int)pos_cs.x, (int)pos_cs.y, spawn_region_ws.w, spawn_region_ws.h };

	mp_set_render_draw_color(c);
	mp_render_draw_rect(&spawn_region_cs);
}

Type_Descriptor night_wave_data_type_descriptors[] = {
	FIELD(Night_Wave_Data, VT_Int, level),
	FIELD(Night_Wave_Data, VT_Int, total_to_spawn),
	FIELD(Night_Wave_Data, VT_Float, max_spawning_cd)
};

void load_horde_data_csv() {

}
