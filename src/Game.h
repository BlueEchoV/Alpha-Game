#pragma once
#include "Entity.h"

struct Game_Data {
	MP_Rect camera;

	Player player;

	Font_Type selected_font;
};

struct Camera {
	V2_F pos;
};

void draw_tile_map(MP_Renderer* renderer);
