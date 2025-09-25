#include "Menu.h"

Font load_font(const char* file_path) {
	Font result = {};

	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);

	DEFER{
		stbi_image_free(data);
	};

	result.image.w = width;
	result.image.h = height;

	int index = 0;
	for (int y = 0; y < result.image.h; y++) {
		for (int x = 0; x < result.image.w; x++) {
			index = (4 * ((y * width) + x));
			// Check if the color is black
			if (data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 0) {
				// Set alpha to 0 (Transparent)
				data[index + 3] = 0;
			}
		}
	}

	// TODO: Hard coded values (Potentially bad)
	// This function is for bitmap fonts so 
	// this could be okay.
	result.char_width = result.image.w / 18;
	result.char_height = result.image.h / 7;

	MP_Texture* temp = mp_create_texture(0, 0, width, height, false);

	mp_set_texture_blend_mode(temp, MP_BLENDMODE_BLEND);

	result.image.texture = temp;
	void* pixels;
	int pitch;
	mp_lock_texture(temp, NULL, &pixels, &pitch);

	// Copy what's in my data into the pixels
	// my_Memory_Copy();
	// memcpy(pixels, data, (width * height) * 4);
	my_mem_copy(data, pixels, (width * height) * 4);

	mp_unlock_texture(temp);

	result.image.texture->pitch = pitch;
	result.image.texture->pixels = pixels;

	result.bitmap_width = 18;
	result.bitmap_height = 7;

	result.char_width = result.image.w / result.bitmap_width;
	result.char_height = result.image.h / result.bitmap_height;

	return result;
}

Font dummy_font = {};
std::unordered_map<Font_Type, Font> fonts;
void load_fonts() {
	dummy_font = load_font("assets\\dummy_font.png");
	fonts[FT_Basic]  = load_font("assets\\basic_font.png");
}

Font* get_font(Font_Type type) {
	auto iterator = fonts.find(type);

	if (iterator != fonts.end()) {
		return &iterator->second;
	} else {
		return &dummy_font;
	}
}

// NOTE: It's important to note here that we are currently drawing outside of the UV coordinates.
// However, setting TexParam to GL_Repeat instead of GL_Mirrored, fixes the issue
void draw_character(Font& font, char character, int x, int y, int size) {
	int ascii = (int)character - (int)' ';
	int src_x = font.char_width * (ascii % font.bitmap_width);
	int src_y = font.char_height * ((font.bitmap_height - 1) - (ascii / font.bitmap_width)) + 1;

	MP_Rect src = { src_x, src_y, font.char_width, font.char_height };
	MP_Rect dst = { x, y, (font.char_width * size), (font.char_height * size) };

	mp_render_copy(font.image.texture, &src, &dst);
}

// Draws in camera space 
void draw_string(Font& font, const char* str, Color_Type c, bool background, int x, int y, int size, bool center_x) {
	int length = (int)strlen(str);

	int final_x = x;
	int final_y = y;

	if (center_x) {
		final_x -= (int)(((float)length / 2.0f) * (float)(font.char_width * size));
		final_y -= (int)(((float)font.char_height * (float)size) / 2.0f);
	}

	if (background) {
		int padding = 3;
		MP_Rect rect = {};
		rect.x = final_x - padding;
		rect.y = final_y - padding;
		rect.w = (length * (font.char_width * size)) + (padding * 2);
		rect.h = (font.char_height * size) + (padding * 2);
		mp_set_render_draw_color(0, 0, 0, 255);
		mp_render_fill_rect(&rect);
	}

	mp_set_texture_color_mod(font.image.texture, c);
	for (int i = 0; i < length; i++) {
		draw_character(font, str[i], final_x + ((font.char_width * size) * i), final_y, size);
	}
	mp_set_texture_color_mod(font.image.texture, 255, 255, 255);
}

void draw_string(Font& font, const char* str, Color_Type c, bool background, float x, float y, int size, bool center_x) {
	draw_string(font, str, c, background, (int)x, (int)y, size, center_x);
}

void draw_string(Font& font, const char* str, Color_Type c, bool background, int x, int y, int size, bool center_x, uint8_t alpha_mod) {
	mp_set_texture_alpha_mod(font.image.texture, alpha_mod);
	draw_string(font, str, c, background, x, y, size, center_x);
	mp_set_texture_alpha_mod(font.image.texture, 255);
}

// Draws in camera space 
void draw_quick_string(Color_Type c, bool background, const char* str, int x, int y) {
	Font* font = get_font(FT_Basic);
	draw_string(*font, str, c, background, x, y, 1, true);
}

// It is used at the start of each button call to determine if the current button was the active one last frame
std::string frame_hot_name = "";
std::string next_frame_hot_name = "";
// Draws centered by default
bool button_text(const MP_Rect& button_area, const std::string& text, int text_size, Font_Type ft, Color_Type text_color, bool draw_background, 
	Color_Type background_color, bool center_button) {
	Font* font = get_font(ft);

	V2 mouse_pos = get_playground_mouse_position(Globals::renderer->open_gl.window_handle);

	V2 center = {};
	MP_Rect draw_rect = {};
	if (center_button) {
		center = {(float)button_area.x, (float)button_area.y};
		draw_rect = button_area;
		draw_rect.x -= button_area.w / 2;
		draw_rect.y -= button_area.h / 2;
	}
	else {
		center = {(float)button_area.x + button_area.w / 2, (float)button_area.y + button_area.h / 2};
		draw_rect = button_area;
	}

	bool was_hot = frame_hot_name == text;

	if (draw_background) {
		mp_set_render_draw_color(background_color);
		mp_render_fill_rect(&draw_rect);
	}

	bool result = false;

	// 1: Check if mouse is within button range
	if (mouse_pos.x >= (center.x - (button_area.w / 2)) && 
		mouse_pos.x <= (center.x + (button_area.w / 2)) && 
		mouse_pos.y >= (center.y - (button_area.h / 2)) &&
		mouse_pos.y <= (center.y + (button_area.h / 2))) {
		mp_set_render_draw_color(CT_Blue);

		// 2: Check if the left mouse button is down
		if (key_pressed_and_held(VK_LBUTTON)) {
			next_frame_hot_name = text;
			mp_set_render_draw_color(CT_Red);
		}

		if (was_hot && !key_pressed_and_held(VK_LBUTTON)) {
			mp_set_render_draw_color(CT_Green);
			result = true;
		}
	}
	else {
		mp_set_render_draw_color(CT_Green);
	}

	mp_render_draw_rect(&draw_rect);

	draw_string(*font, text.c_str(), text_color, false, (float)center.x, (float)center.y, text_size, true);

	// std::string temp = std::to_string(center.x) + " " + std::to_string(center.y);
	// draw_string(*font, temp.c_str(), CT_Dark_Yellow, false, center.x, center.y, string_size, true);

	return result;
}

bool button_image(const MP_Rect& button_area, const std::string& sprite_sheet_name, MP_Rect* texture_src_rect, bool center_button) {
	V2 center = {};
	MP_Rect draw_rect = {};
	if (center_button) {
		center = {(float)button_area.x, (float)button_area.y};
		draw_rect = button_area;
		draw_rect.x -= button_area.w / 2;
		draw_rect.y -= button_area.h / 2;
	}
	else {
		center = {(float)button_area.x + button_area.w / 2, (float)button_area.y + button_area.h / 2};
		draw_rect = button_area;
	}

	MP_Point point = { (int)center.x, (int)center.y };

	Sprite_Sheet* ss = get_sprite_sheet(sprite_sheet_name);
	// TODO: Make this able to play sprite sheets?
	mp_render_copy_ex(ss->sprites[0].image.texture, texture_src_rect, &draw_rect, 0, &point, SDL_FLIP_NONE);

	return(button_text(draw_rect, "image_name", 0, FT_None, CT_Black, false, CT_Black, center_button));
}
