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

struct World {
	Tile_Map map;

	MP_Texture* tex_1;
	MP_Texture* tex_2;
	MP_Texture* noise_tex;
};

void get_tile_grid_index_from_pos_ws(V2 pos_ws, int& tile_grid_index_x, int& tile_grid_index_y);
V2 get_tile_pos_ws_from_grid_index(int tile_grid_index_x, int tile_grid_index_y);
V2 get_tile_pos_ws_from_pos_ws(V2 pos_ws);

// By default, the centers of the map are 0,0
Tile_Map create_tile_map(int w_in_tiles, int h_in_tiles);

World create_world(int w, int h, MP_Texture* tex_1, MP_Texture* tex_2, MP_Texture* noise_tex);
// Check tilemap collision
// bool check_player_collision_with_tile_map();
void draw_entire_world(Camera& camera, World& world);

// void load_tile_map_csv();
