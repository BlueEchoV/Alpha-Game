#pragma once
#include "Utility.h"
#include "Sprite_Sheet.h"
#include "Game_Data.h"
#include "Debug.h"

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

};

void draw_entire_map(Camera camera);

// void load_tile_map_csv();
