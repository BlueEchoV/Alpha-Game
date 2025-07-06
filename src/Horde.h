#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"
#include "Tile_Map.h"
#include "Unit.h"
#include "Player.h"

enum Horde_Type {
	HT_Not_Specified,
	HT_Basic
};

enum Spawn_Direction {
	SD_North,
	SD_South,
	SD_East,
	SD_West
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
	Spawn_Direction spawn_direction;
	int spawn_region_size_in_tiles;
	bool begin_spawning;
	int total_to_spawn;
	int total_spawned;

	Cooldown spawning_cd;
}; 

Horde create_horde(Faction faction, Horde_Type ht, Spawn_Direction spawn_direction, int spawn_region_size_in_tiles);
void spawn_and_update_horde(const std::string& unit_name, std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage, 
	Horde& horde, Player& player, Tile_Map& tile_map, float delta_time);

void draw_horde_spawn_region(Color_Type c, Horde& horde, Tile_Map& tile_map, V2 camera_pos);
