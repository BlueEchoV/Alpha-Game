#pragma once
#include "Entity.h"

enum Debug_Image {
    DI_mp_render_fill_rect,
    DI_mp_render_fill_rects,
	DI_mp_render_draw_line,
	DI_mp_render_draw_lines,
	DI_mp_render_draw_point,
	DI_mp_render_draw_points,
	DI_mp_render_draw_rect,
	DI_mp_render_draw_rects,
	DI_mp_render_copy,
	DI_mp_render_copy_alpha,
	DI_2d_matrix_transformation_rect,
	DI_copy_ex,
    DI_Count
};

Camera create_camera(Player& player);
void update_camera(Camera& camera, Player& player);

void draw_circle(Color_Type c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines);
void debug_draw_coor_cs(Color_Type c, bool background, V2 camera_pos, int x, int y, bool coordinates_are_in_ws);
void debug_draw_coor_ws(Color_Type c, bool background, V2 camera_pos, int x, int y, bool coordinates_are_in_cs);
void draw_debug_info(Game_Data& game_data, Font& font, MP_Texture* debug_texture);

void draw_tile(Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency);
void fire_player_arrow(Game_Data& game_data, Image_Type it, int arrow_w, int arrow_h, int speed);
void render(Game_Data& game_data);
