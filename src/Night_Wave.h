#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"
#include "World.h"
#include "Unit.h"
#include "Player.h"

enum Night_Wave_Type {
	NWT_Not_Specified,
	NWT_Basic
};

enum Spawn_Direction {
	SD_North,
	SD_South,
	SD_East,
	SD_West
};

struct Night_Wave_Data {
	int level;
	int total_to_spawn;
	// In seconds
	float max_spawning_cd;
	// Zombie Types
};

struct Night_Wave {
	Faction faction;
	int level;
	Spawn_Direction spawn_direction;
	int spawn_region_size_in_tiles;
	bool begin_spawning;
	int total_to_spawn;
	int total_spawned;

	Cooldown spawning_cd;
}; 

Night_Wave create_night_wave(Faction faction, Night_Wave_Type nwt, Spawn_Direction spawn_direction, int spawn_region_size_in_tiles);
void spawn_and_update_night_wave(const std::string& unit_name, std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage, 
	Night_Wave& night_wave, Player& player, Tile_Map& tile_map, float delta_time);

void draw_night_wave_spawn_region(Color_Type c, Night_Wave& night_wave, Tile_Map& tile_map, V2 camera_pos);
