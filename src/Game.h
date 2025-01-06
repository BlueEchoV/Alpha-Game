#pragma once
#include "Entity.h"

struct Game_Data {
	Player player;
};

struct Camera {
	V2_F pos;
};

void draw_tile_map(MP_Renderer* renderer);
