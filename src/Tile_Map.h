#pragma once
#include "Utility.h"
#include "Image.h"
#include "Game_Data.h"

struct Tile {
	int type;
	std::string texture_name;
};

// NOTE: Have the map bound to the size of my screen?
// NOTE: Put into excel
// NOTE: Anything outside of this region could just be forest or whatever the map type's 
// default trees are or whatever
int tile_entites[100] = {
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,

	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0,
};

void spawn_environment();

void draw_tile(Camera camera, int tile_index_x, int tile_index_y, float noise_frequency);

void draw_tile_map(Camera camera);

// void load_tile_map_csv();
