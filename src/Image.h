#pragma once
#include "Renderer.h"
#include <unordered_map>

struct Image {
	int w;
	int h;
	MP_Texture* texture;
};

struct Font {
	int char_width;
	int char_height;

	int bitmap_width;
	int bitmap_height;

	Image image;
};

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

enum Tile_Type {
	TT_Rock,
	TT_Grass,
	TT_Water,
	TT_Total
};

enum Image_Type {
	IT_Sun,
	IT_Dummy_Player,
	IT_Player_Rugged_Male,
	IT_Enemy_Clothed_Zombie,
	IT_Top_Down_Player_Demo,
	IT_Arrow_1,
	IT_Dummy_Tile_32x32,
	IT_Rock_32x32,
	IT_Grass_32x32,
	IT_Water_32x32
};

enum Font_Type {
	FT_Basic
};

Image load_image(const char* file_path);

extern std::unordered_map<Image_Type, Image> images;
void load_images();
Image* get_image(Image_Type image_type);

extern std::unordered_map<Font_Type, Font> fonts;
void load_fonts();
Font* get_font(Font_Type type);

Font load_font(const char* file_path);
void draw_character(Font& font, char character, int x, int y, int size, int background);
void draw_string(Font& font, const char* str, int x, int y, int size, bool center_x, bool background);
void draw_string(Font& font, const char* str, float x, float y, int size, bool center_x, bool background);
void draw_string(const char* str, int x, int y);

void draw_debug_2d_rotation_matrix_rect(V2 center, Font* font);
void draw_mp_library_debug_images(Font& font, MP_Texture* debug_texture, bool toggle_debug_images);
