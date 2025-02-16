#pragma once
#include "Renderer.h"
#include <unordered_map>

struct Image {
	int w;
	int h;

	// This is the distance from the center of the image to 
	// the farthest non transparent pixel in the sprite.
	float sprite_radius;

	MP_Texture* texture;
};

struct Font {
	int char_width;
	int char_height;

	int bitmap_width;
	int bitmap_height;

	Image image;
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
void draw_character(Font& font, char character, int x, int y, int size);
void draw_string(Font& font, const char* str, Color_Type c, bool background, int x, int y, int size, bool center_x);
void draw_string(Font& font, const char* str, Color_Type c, bool background, float x, float y, int size, bool center_x);
void draw_quick_string(Color_Type c, bool background, const char* str, int x, int y);

void draw_debug_2d_rotation_matrix_rect(V2 center, Font* font, float delta_time);
