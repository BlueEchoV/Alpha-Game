#pragma once
#include "Utility.h"
#include "Sprite_Sheet.h"
#include "Game_Utility.h"

enum Environment_Entity : int {
	EE_Empty,
	EE_Tree, 
	EE_Rock,
	EE_Bush
};

struct Tile {
	int type;
	std::string texture_name;
};

struct Tile_Map {
	int w;
	int h;
};

V2 get_tile_pos_ws(int tile_index_x, int tile_index_y);
// By default, the centers of the map are 0,0
Tile_Map create_tile_map(int w, int h);
// Check tilemap collision
// bool check_player_collision_with_tile_map();
void draw_entire_map(Camera& camera, Tile_Map& tile_map);

// void load_tile_map_csv();
