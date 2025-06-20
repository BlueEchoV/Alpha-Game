#pragma once
#include "Utility.h"
#include "Sprite_Sheet.h"
#include "Game_Data.h"

enum Environment_Entity : int {
	EE_Empty,
	EE_Tree, 
	EE_Rock
};

struct Tile {
	int type;
	std::string texture_name;
};

void draw_entire_map(Camera camera);

// void load_tile_map_csv();
