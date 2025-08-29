#pragma once
#include "Utility.h"
#include "Sprite_System.h"
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

V2 get_tile_pos_ws(int tile_index_x, int tile_index_y);
V2 get_tile_pos_index(V2 pos_ws);
// By default, the centers of the map are 0,0
Tile_Map create_tile_map(int w, int h);
// Check tilemap collision
// bool check_player_collision_with_tile_map();
void draw_entire_map(Camera& camera, Tile_Map& tile_map, MP_Texture* texture_1, MP_Texture* texture_2, MP_Texture* noise_texture);

// void load_tile_map_csv();
