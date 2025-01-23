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

void debug_draw_circle(Color c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines);
void debug_draw_coor(V2 camera_pos, int x_ws, int y_ws);
void debug_draw_line();

void draw_tile(Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency);
void fire_player_arrow(Game_Data& game_data, Image_Type it, int arrow_w, int arrow_h, int speed);
void render(Game_Data& game_data);
