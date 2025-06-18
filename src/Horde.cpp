#include "Horde.h"

Horde_Data bad_horde_data = {
	// level		total_zombies		spawning_cd
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

// Call this at the end of every day?
Horde create_horde(Faction faction, Horde_Type ht, MP_Rect rect) {
	Horde result = {};

	Horde_Data data = get_horde_data(ht);

	result.faction = faction;
	result.level = data.level;
	result.spawning_cd = create_cooldown(data.max_spawning_cd);
	result.spawn_region_ws = rect;
	result.begin_spawning = false;
	result.total_to_spawn = data.total_to_spawn;
	result.total_spawned = 0;

	return result;
}

void spawn_and_update_horde(std::vector<Handle> unit_handles, Storage<Unit>& unit_storage, Horde& horde, Player& player, float delta_time) {
	if (horde.begin_spawning) {
		if (horde.total_spawned < horde.total_to_spawn) {
			if (check_and_update_cooldown(horde.spawning_cd, delta_time)) {
				if (horde.faction == F_Enemies) {
					V2 random_pos_ws = {};
					MP_Rect spawn_region_ws = horde.spawn_region_ws;
					random_pos_ws.x = (float)((int)spawn_region_ws.x + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.w));
					random_pos_ws.y = (float)((int)spawn_region_ws.y + (rand() * (int)(delta_time * 1000.0f) % spawn_region_ws.h));
					log("Random Position: x = %f, y = %f", random_pos_ws.x, random_pos_ws.y);
					spawn_unit(
						horde.faction,
						"zombie_woman",
						AS_Running,
						unit_storage,
						unit_handles,
						&player,
						random_pos_ws
					);
					horde.spawning_cd.current++;
				}
			}
		}
	}
}

// NOTE: Draw diagnal lines? Or dotted lines?
void draw_horde_spawn_region(Color_Type c, Horde& horde, V2 camera_pos) {
	V2 pos_ws = {(float)horde.spawn_region_ws.x, (float)horde.spawn_region_ws.y};
	V2 pos_cs = convert_ws_to_cs(pos_ws, camera_pos);
	MP_Rect cs_rect = { (int)pos_cs.x, (int)pos_cs.y, horde.spawn_region_ws.w, horde.spawn_region_ws.h };

	mp_set_render_draw_color(c);
	mp_render_draw_rect(&cs_rect);
}

