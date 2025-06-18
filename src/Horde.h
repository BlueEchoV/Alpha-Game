#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"

#include "Unit.h"
#include "Player.h"

enum Horde_Type {
	HT_Not_Specified,
	HT_Basic
};

struct Horde_Data {
	int level;
	int total_to_spawn;
	// In seconds
	float max_spawning_cd;
	// Zombie Types
};

struct Horde {
	Faction faction;
	int level;
	MP_Rect spawn_region_ws;
	bool begin_spawning;
	int total_to_spawn;
	int total_spawned;

	Cooldown spawning_cd;
}; 

Horde create_horde(Faction faction, Horde_Type ht, MP_Rect rect);
void spawn_and_update_horde(std::vector<Handle> unit_handles, Storage<Unit>& unit_storage, Horde& horde, Player& player, float delta_time);
void draw_horde_spawn_region(Color_Type c, Horde& horde, V2 camera_pos);


