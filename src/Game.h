#pragma once
#include "Entity.h"

struct Game_Data {
	MP_Rect camera;

	Player player;
	std::vector<Arrow> arrows;

	Font_Type selected_font;
};

struct Camera {
	V2 pos;
};

void draw_tile(MP_Renderer* renderer, Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency);
void fire_arrow(Game_Data& game_data, Image_Type it, int speed, V2 target, V2 origin);
void render(MP_Renderer* renderer, Game_Data& game_data);
