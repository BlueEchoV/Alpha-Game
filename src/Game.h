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

void debug_draw_coor(Game_Data& game_data, V2 coor_to_draw, bool convert_coor_to_draw_to_cs,
	V2 draw_at, bool convert_draw_at_to_cs, Color_Type c, bool background, std::string custom_text);
void debug_draw_collider_coodinates(Game_Data& game_data, Rigid_Body& rb);
void debug_draw_mp_renderer_visualizations(Font& font, MP_Texture* debug_texture, float delta_time);
void debug_draw_stats(Font& font, MP_Texture* debug_texture);
void debug_draw_all_debug_info(Game_Data& game_data, Font& font, MP_Texture* debug_texture, float delta_time);

void draw_tile(Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency);
void render(Game_Data& game_data, float delta_time);
