#pragma once

#include "Renderer.h"

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
    DI_Count
};

Image load_image(MP_Renderer* renderer, const char* file_path);
Font load_font(MP_Renderer* renderer, const char* file_path);
void draw_character(MP_Renderer* renderer, Font& font, char character, int x, int y, int size, int background);
void draw_string(MP_Renderer* renderer, Font& font, const char* str, int x, int y, int size, bool center_x, bool background);
void draw_mp_library_debug_images(MP_Renderer* renderer, Font& font, MP_Texture* debug_texture, bool toggle_debug_images);
