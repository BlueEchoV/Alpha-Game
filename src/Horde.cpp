#include "Horde.h"

Horde_Data bad_horde_data = {
	// level		total_enemies		spawning_cd
	   1,			20,					0.5f
};

std::unordered_map<std::string, Horde_Data> horde_data_map;

Horde_Data get_horde_data(Horde_Type horde_type) {
	Horde_Data result = {};

	if (horde_type == HT_Not_Specified) {
		return bad_horde_data;
	}
	return bad_horde_data;
};

MP_Rect get_spawn_region_ws(Horde& horde, Tile_Map& tile_map) {
	MP_Rect result = {};

	switch (horde.spawn_direction) {
	case SD_North: {
		result.x = -(tile_map.w / 2);
		result.y = tile_map.h / 2;
		result.w = tile_map.w;
		result.h = horde.spawn_region_size_in_tiles;
		break;
	}
	case SD_South: {
		result.x = -(tile_map.w / 2);
		result.y = -(tile_map.h / 2) - horde.spawn_region_size_in_tiles;
		result.w = tile_map.w;
		result.h = horde.spawn_region_size_in_tiles;
		break;
	}
	case SD_East: {
		result.x = tile_map.w - (tile_map.w / 2);
		result.y = -(tile_map.h / 2);
		result.w = horde.spawn_region_size_in_tiles;
		result.h = tile_map.h;
		break;
	}
	case SD_West: {
		result.x = -(tile_map.w / 2) - horde.spawn_region_size_in_tiles;
		result.y = -(tile_map.h / 2);
		result.w = horde.spawn_region_size_in_tiles;
		result.h = tile_map.h;
		break;
	}
	default: {
		log("Error: Spawn region not specified");
		break;
	}
	}

	V2 pos_ws = get_tile_pos_ws(result.x, result.y);
	result.x = (int)pos_ws.x;
	result.y = (int)pos_ws.y;
	result.w = result.w * Globals::tile_w;
	result.h = result.h * Globals::tile_h;
	return result;
}

// Call this at the end of every day?
Horde create_horde(Faction faction, Horde_Type ht, Spawn_Direction spawn_direction, int spawn_region_size_in_tiles) {
	Horde result = {};

	Horde_Data data = get_horde_data(ht);

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

void spawn_and_update_horde(const std::string& unit_name, std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage, 
	Horde& horde, Player& player, Tile_Map& tile_map, float delta_time) {
	if (horde.begin_spawning) {
		if (horde.total_spawned < horde.total_to_spawn) {
			if (check_and_update_cooldown(horde.spawning_cd, delta_time)) {
				if (horde.faction == F_Enemies) {
					V2 random_pos_ws = {};
					MP_Rect spawn_region_ws = get_spawn_region_ws(horde, tile_map);
					random_pos_ws.x = (float)((int)spawn_region_ws.x + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.w));
					random_pos_ws.y = (float)((int)spawn_region_ws.y + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.h));
					log("Random Position: x = %f, y = %f", random_pos_ws.x, random_pos_ws.y);
					spawn_unit(
						horde.faction,
						unit_name,
						AS_Walking,
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
void draw_horde_spawn_region(Color_Type c, Horde& horde, Tile_Map& tile_map, V2 camera_pos) {
	MP_Rect spawn_region_ws = get_spawn_region_ws(horde, tile_map);
	V2 pos_cs = convert_ws_to_cs({(float)spawn_region_ws.x, (float)spawn_region_ws.y}, camera_pos);
	MP_Rect spawn_region_cs = { (int)pos_cs.x, (int)pos_cs.y, spawn_region_ws.w, spawn_region_ws.h };

	mp_set_render_draw_color(c);
	mp_render_draw_rect(&spawn_region_cs);
}

Type_Descriptor horde_data_type_descriptors[] = {
	FIELD(Horde_Data, VT_Int, level),
	FIELD(Horde_Data, VT_Int, total_to_spawn),
	FIELD(Horde_Data, VT_Float, max_spawning_cd)
};

void load_horde_data_csv() {

}
