#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"
#include "World.h"
#include "Unit.h"
#include "Player.h"

enum Map_Difficulty {
	MD_Not_Specified, 
	MD_Normal,
	MD_Hard,
	MD_Endless
};

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
	Map_Difficulty difficulty;
	int current_wave;
	Spawn_Direction spawn_direction;
	int spawn_region_size_in_tiles;
	bool begin_spawning;
	int total_to_spawn;
	int total_spawned;

	Cooldown spawning_cd;
}; 

int get_max_waves_from_difficulty(Map_Difficulty md);
Night_Wave create_night_wave(Map_Difficulty difficulty, Spawn_Direction spawn_direction, int spawn_region_size_in_tiles,
	int current_wave, int enemies_to_spawn);
void spawn_and_update_night_wave(Night_Wave& night_wave, int& active_enemy_units, Player& player, Tile_Map& tile_map, float delta_time,
	std::vector<Handle>& unit_handles, Storage<Unit>& unit_storage,
	Storage<Draw_Order>& draw_order_storage, std::vector<Handle>& draw_order_handles);

void draw_night_wave_spawn_region(Color_Type c, Night_Wave& night_wave, Tile_Map& tile_map, V2 camera_pos);

