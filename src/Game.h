#pragma once
#include "Entity.h"

struct Game_Data {
	MP_Rect camera;

	Player player;
	Arrow arrow;

	Font_Type selected_font;
};

struct Camera {
	V2 pos;
};

void draw_tile(MP_Renderer* renderer, Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency);
void render(MP_Renderer* renderer, Game_Data& game_data);
