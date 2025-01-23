#pragma once
#include "Entity.h"

struct Camera {
	V2 pos_ws;
	int w, h;
};

struct Game_Data {
	Camera camera;

	Player player;
	std::vector<Arrow> arrows;

	Font_Type selected_font;
};

void draw_circle(MP_Renderer* renderer, Color c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines);
void draw_tile(MP_Renderer* renderer, Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency);
void fire_player_arrow(MP_Renderer* renderer, Game_Data& game_data, Image_Type it, int arrow_w, int arrow_h, int speed);
void render(MP_Renderer* renderer, Game_Data& game_data);
