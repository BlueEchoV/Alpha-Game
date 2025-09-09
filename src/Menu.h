#pragma once
#include "Renderer.h"
#include "Sprite_System.h"

struct Font {
	int char_width;
	int char_height;

	int bitmap_width;
	int bitmap_height;

	Image image;
};

enum Font_Type {
	FT_None,
	FT_Basic 
};

extern std::unordered_map<Font_Type, Font> fonts;
void load_fonts();
Font* get_font(Font_Type type);
Font load_font(const char* file_path);

void draw_character(Font& font, char character, int x, int y, int size);
void draw_string(Font& font, const char* str, Color_Type c, bool background, int x, int y, int size, bool center_x);
void draw_string(Font& font, const char* str, Color_Type c, bool background, float x, float y, int size, bool center_x);
void draw_quick_string(Color_Type c, bool background, const char* str, int x, int y);

extern std::string frame_hot_name;
extern std::string next_frame_hot_name;
bool button_text(const MP_Rect& button_area, const std::string& text, int text_size, Font_Type ft, Color_Type text_color, bool draw_background,
	Color_Type background_color, bool center_button);
bool button_image(const MP_Rect& button_area, const std::string& sprite_sheet_name, MP_Rect* texture_src_rect, bool center_button);
