#include "Image.h"

Image load_image(MP_Renderer* renderer, const char* file_path) {
	Image result = {};

	// Might be necessary for opengl, testing soon
	stbi_set_flip_vertically_on_load(true); 

	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);
	if (data == NULL) {
		log("Error: stbi_load is null: %s", file_path);
	}

	DEFER{
		stbi_image_free(data);
	};

	result.w = width;
	result.h = height;

	result.texture = mp_create_texture(renderer, 0, 0, result.w, result.h);

	void* pixels;
	int pitch;
	mp_lock_texture(result.texture, NULL, &pixels, &pitch);

	my_mem_copy(data, pixels, ((width * height) * channels));

	mp_unlock_texture(result.texture);
	
	return result;
}

Font load_font(MP_Renderer* renderer, const char* file_path) {
	Font result = {};

	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);

	DEFER{
		stbi_image_free(data);
	};

	result.image.w = width;
	result.image.h = height;

	for (int y = 0; y < result.image.h; y++) {
		for (int x = 0; x < result.image.w; x++) {
			int index = 0;
			index = (4 * ((y * width) + x));
			// Check if the color is black
			if (data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 0) {
				// Set alpha to 0 (Transparent)
				data[index + 3] = 0;
			}
		}
	}

	// Hard coded values (Potentially bad)
	// This function is for bitmap fonts so 
	// this could be okay.
	result.char_width = result.image.w / 18;
	result.char_height = result.image.h / 7;

	MP_Texture* temp = mp_create_texture(renderer, 0, 0, width, height);

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

	result.bitmap_width = 18;
	result.bitmap_height = 7;

	result.char_width = result.image.w / result.bitmap_width;
	result.char_height = result.image.h / result.bitmap_height;

	return result;
}

Image dummy_image = {};
std::unordered_map<Image_Type, Image> images;
void load_images(MP_Renderer* renderer) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}

	// Init the dummy image
	dummy_image = load_image(renderer, "assets\\dummy_image.png");
	assert(&dummy_image != NULL);

	images[IT_Sun] = load_image(renderer, "assets\\sun.png");
	images[IT_Dummy_Player] = load_image(renderer, "assets\\dummy_player.png");
	images[IT_Player_Rugged_Male] = load_image(renderer, "assets\\player_rugged_male.png");
	images[IT_Enemy_Clothed_Zombie] = load_image(renderer, "assets\\enemy_clothed_zombie.png");
	images[IT_Top_Down_Player_Demo] = load_image(renderer, "assets\\top_down_player_demo.png");
	images[IT_Arrow_1] = load_image(renderer, "assets\\arrow_1.png");
	images[IT_Dummy_Tile_32x32] = load_image(renderer, "assets\\dummy_tile_32x32.png");
	images[IT_Rock_32x32] = load_image(renderer, "assets\\rock_32x32.png");
	images[IT_Grass_32x32] = load_image(renderer, "assets\\grass_32x32.png");
	images[IT_Water_32x32] = load_image(renderer, "assets\\water_32x32.png");
}

Image* get_image(Image_Type image_type) {
	auto iterator = images.find(image_type);

	if (iterator != images.end()) {
		return &iterator->second;
	} else {
		// Return the dummy image so it doesn't crash the game
		return &dummy_image;
	}
}

Font dummy_font = {};
std::unordered_map<Font_Type, Font> fonts;
void load_fonts(MP_Renderer* renderer) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}

	dummy_font = load_font(renderer, "assets\\dummy_font.png");
	fonts[FT_Basic]  = load_font(renderer, "assets\\basic_font.png");
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
void draw_character(MP_Renderer* renderer, Font& font, char character, int x, int y, int size, int background) {
	REF(background);
	int ascii = (int)character - (int)' ';
	int src_x = font.char_width * (ascii % font.bitmap_width);
	int src_y = font.char_height * ((font.bitmap_height - 1) - (ascii / font.bitmap_width)) + 1;

	MP_Rect src = { src_x, src_y, font.char_width, font.char_height};
	MP_Rect dst = { x, y, (font.char_width * size), (font.char_height * size)};

	mp_render_copy(renderer, font.image.texture, &src, &dst);
}

void draw_string(MP_Renderer* renderer, Font& font, const char* str, int x, int y, int size, bool center_x, bool background) {
	if (renderer == NULL) {
		log("Error: Renderer is null");
		return;
	}
	int length = (int)strlen(str);

	int final_x = x;
	int final_y = y;

	if (center_x) {
		final_x -= (int)(((float)length / 2.0f) * (float)(font.char_width * size));
		// final_y -= (int)(((float)font.char_height * (float)size) / 2.0f);
	}

	int padding = 3;
	MP_Rect rect = {};
	rect.x = final_x - padding;
	rect.y = final_y - padding;
	rect.w = (length * (font.char_width * size)) + (padding * 2);
	rect.h = (font.char_height * size) + (padding * 2);
	mp_set_render_draw_color(renderer, 0, 0, 0, 255);
	mp_render_fill_rect(renderer, &rect);

	for (int i = 0; i < length; i++) {
		draw_character(renderer, font, str[i], final_x + ((font.char_width * size) * i), final_y, size, background);
	}
}

void draw_string(MP_Renderer* renderer, Font& font, const char* str, float x, float y, int size, bool center_x, bool background) {
	draw_string(renderer, font, str, (int)x, (int)y, size, center_x, background);
}

void draw_string(MP_Renderer* renderer, Font& font, const char* str, int x, int y) {
	draw_string(renderer, font, str, x, y, 1, true, true);
}

int window_w = 0;
int window_h = 0;
void draw_debug_2d_rotation_matrix_rect(MP_Renderer* renderer, V2 center, Font* font) {
	V2 c = {};
	c.x = center.x;
	c.y = center.y;

	float w = 100;
	float h = 100;

	bool recalculate_points = false;
	if (window_w != renderer->window_width || 
		window_h != renderer->window_height) {
		recalculate_points = true;
	}
	window_w = renderer->window_width;
	window_h = renderer->window_height;

	V2 original_top_left = { c.x - (w / 2.0f), (int)c.y + (h / 2) };
	V2 original_top_right = { c.x + (w / 2.0f), (int)c.y + (h / 2) };
	V2 original_bottom_right = { c.x + (w / 2.0f), (int)c.y - (h / 2) };
	V2 original_bottom_left = { c.x - (w / 2.0f), (int)c.y - (h / 2) };

	static V2 new_top_left = original_top_left;
	static V2 new_top_right = original_top_right;
	static V2 new_bottom_right = original_bottom_right;
	static V2 new_bottom_left = original_bottom_left;

	static float angle = 0.0f;
	static float last_angle = 0.0f;
	float rotation_speed = 1.0f;
	if (key_pressed_and_held(KEY_A)) {
		angle += rotation_speed;
	}
	if (key_pressed_and_held(KEY_D)) {
		angle -= rotation_speed;
	}
	if (angle != last_angle || recalculate_points) {
		new_top_left = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_top_left.x, (float)original_top_left.y);
		new_top_right = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_top_right.x, (float)original_top_right.y);
		new_bottom_right = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_bottom_right.x, (float)original_bottom_right.y);
		new_bottom_left = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_bottom_left.x, (float)original_bottom_left.y);
		last_angle = angle;
	}
	int string_size = 1;
	std::string center_string = {};
	center_string = "(" + std::to_string((int)c.x) + " " + std::to_string((int)c.y) + ")";
	draw_string(renderer, *font, center_string.c_str(), (int)c.x, (int)c.y, string_size, true, false);

	std::string angle_string = {};
	center_string = std::to_string((int)angle);
	draw_string(renderer, *font, center_string.c_str(), (int)c.x, (int)c.y + (font->char_height * 2) * string_size, string_size, true, false);

	std::string top_left_string = {};
	top_left_string = "(" + std::to_string((int)new_top_left.x) + " " + std::to_string((int)new_top_left.y) + ")";
	draw_string(renderer, *font, top_left_string.c_str(), new_top_left.x, new_top_left.y, string_size, true, false);

	std::string top_right_string = {};
	top_right_string = "(" + std::to_string((int)new_top_right.x) + " " + std::to_string((int)new_top_right.y) + ")";
	draw_string(renderer, *font, top_right_string.c_str(), new_top_right.x, new_top_right.y, string_size, true, false);

	std::string bottom_right_string = {};
	bottom_right_string = "(" + std::to_string((int)new_bottom_right.x) + " " + std::to_string((int)new_bottom_right.y) + ")";
	draw_string(renderer, *font, bottom_right_string.c_str(), new_bottom_right.x, new_bottom_right.y, string_size, true, false);

	std::string bottom_left_string = {};
	bottom_left_string = "(" + std::to_string((int)new_bottom_left.x) + " " + std::to_string((int)new_bottom_left.y) + ")";
	draw_string(renderer, *font, bottom_left_string.c_str(), new_bottom_left.x, new_bottom_left.y, string_size, true, false);

	mp_set_render_draw_color(renderer, C_Orange);
	mp_render_draw_line(renderer, (int)new_top_left.x, (int)new_top_left.y, (int)new_top_right.x, (int)new_top_right.y);
	mp_render_draw_line(renderer, (int)new_top_right.x, (int)new_top_right.y, (int)new_bottom_right.x, (int)new_bottom_right.y);
	mp_render_draw_line(renderer, (int)new_bottom_right.x, (int)new_bottom_right.y, (int)new_bottom_left.x, (int)new_bottom_left.y);
	mp_render_draw_line(renderer, (int)new_bottom_left.x, (int)new_bottom_left.y, (int)new_top_left.x, (int)new_top_left.y);
}

void draw_mp_library_debug_images(MP_Renderer* renderer, Font& font, MP_Texture* debug_texture, bool toggle_debug_images) {
	if (!toggle_debug_images) { 
		return;
	}

    int width = 150;
    int height = 150;

    int starting_x = 50;
    int starting_y = renderer->window_height - (50 + height);

    int offset_x = 20 + width;
    int offset_y = -50 - height;

    int images_per_row = 5;
	int string_size = 2;

	MP_Rect background_rects[DI_Count] = {};
	int background_padding = 6;

    mp_set_render_draw_color(renderer, 0, 0, 0, 255);
	for (int i = 0; i < DI_Count; i++) {
        int x = (starting_x + (i % images_per_row) * offset_x);
        int y = starting_y + (i / images_per_row) * offset_y; 

		background_rects[i].x = x - background_padding;
		background_rects[i].y = y - background_padding;
		background_rects[i].w = width + background_padding * 2;
		background_rects[i].h = (height + background_padding * 2) + (font.char_height * (string_size + 1)) + 5;
	}
	mp_render_fill_rects(renderer, background_rects, DI_Count);

	// First rows
    for (int i = 0; i < DI_Count; i++) {
        int x = (starting_x + (i % images_per_row) * offset_x);
        int y = starting_y + (i / images_per_row) * offset_y; 
		int title_x = x + width / 2;
		int title_y = y + (font.char_height * (string_size - 1)) + (height);

        MP_Rect rect = {x, y, width, height};

        switch ((Debug_Image)i) {
		case DI_mp_render_fill_rect: {
			draw_string(renderer, font, "fill_rect", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Red);
			mp_render_fill_rect(renderer, &rect);
			break;
		}
		case DI_mp_render_fill_rects: {
			draw_string(renderer, font, "fill_rects", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Green);
			MP_Rect bottom_left_and_top_right[2] = {
				{x		      , y             , width / 2, height / 2}, 
				{x + width / 2, y + height / 2, width / 2, height / 2}
			};
			mp_render_fill_rects(renderer, bottom_left_and_top_right, ARRAYSIZE(bottom_left_and_top_right));

			mp_set_render_draw_color(renderer, C_Blue);
			MP_Rect bottom_right_and_top_left[2] = {
				{x + width / 2, y			  , width / 2, height / 2}, 
				{x            , y + height / 2, width / 2, height / 2} 
			};
			mp_render_fill_rects(renderer, bottom_right_and_top_left, ARRAYSIZE(bottom_right_and_top_left));
			break;
		}
		case DI_mp_render_draw_line: {
			draw_string(renderer, font, "draw_line", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Red);
			mp_render_draw_line(renderer, x, y, x + width, y);

			mp_set_render_draw_color(renderer, C_Green);
			mp_render_draw_line(renderer, x, y + height / 2, x + width, y + height / 2);

			mp_set_render_draw_color(renderer, C_Blue);
			mp_render_draw_line(renderer, x, y + height, x + width, y + height);

			mp_set_render_draw_color(renderer, C_Red);
			mp_render_draw_line(renderer, x, y, x, y + height);

			mp_set_render_draw_color(renderer, C_Green);
			mp_render_draw_line(renderer, x + width / 2, y, x + width / 2, y + height);

			mp_set_render_draw_color(renderer, C_Blue);
			mp_render_draw_line(renderer, x + width, y, x + width, y + height);
			break;
		}
		case DI_mp_render_draw_lines: {
			draw_string(renderer, font, "draw_lines", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Dark_Yellow);

			const int total_points = 5;
			MP_Point points[total_points];

			points[0].x = x;
			points[0].y = y;

			points[1].x = x;
			points[1].y = y + height;

			points[2].x = x + width;
			points[2].y = y + height;

			points[3].x = x + width;
			points[3].y = y;

			points[4].x = x;
			points[4].y = y;

			mp_render_draw_lines(renderer, points, total_points);
			break;
		}
		case DI_mp_render_draw_point: {
			draw_string(renderer, font, "draw_point", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Red);
			mp_render_draw_point(renderer, x		, y			);
			mp_render_draw_point(renderer, x + width, y			);
			mp_render_draw_point(renderer, x		, y + height);
			mp_render_draw_point(renderer, x + width, y + height);

			mp_render_draw_point(renderer, x + width / 4,		  y + height / 4);
			mp_render_draw_point(renderer, x + ((width / 4) * 3), y + height / 4);
			mp_render_draw_point(renderer, x + ((width / 4) * 3), y + ((height / 4) * 3));
			mp_render_draw_point(renderer, x + width / 4,         y + ((height / 4) * 3));
			break;
		}
		case DI_mp_render_draw_points: {
			draw_string(renderer, font, "draw_points", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Green);
			const int grid_size = 4;
			const int total_points = grid_size * grid_size;
			MP_Point points[total_points] = {};

			int x_spacing = (int)((float)width / ((float)grid_size - 1.0f));
			int y_spacing = (int)((float)height / ((float)grid_size - 1.0f));

			// Populate the points array
			int index = 0;
			for (int z = 0; z < grid_size; ++z) {
				for (int j = 0; j < grid_size; ++j) {
					points[index++] = { x + j * x_spacing, y + z * y_spacing };
				}
			}

			mp_render_draw_points(renderer, points, total_points);
			break;
		}
		case DI_mp_render_draw_rect: {
			draw_string(renderer, font, "draw_rect", title_x, title_y, string_size, true, true);
			mp_set_render_draw_color(renderer, C_Red);

			MP_Rect draw_rect = {};
			draw_rect.x = x;
			draw_rect.y = y;
			draw_rect.w = width;
			draw_rect.h = height;

			mp_render_draw_rect(renderer, &draw_rect);
			break;
		}
		case DI_mp_render_draw_rects: {
			draw_string(renderer, font, "draw_rect", title_x, title_y, string_size, true, true);

			mp_set_render_draw_color(renderer, C_Dark_Yellow);

			const int total_rects = 4;
			MP_Rect draw_rects[total_rects] = {};

			draw_rects[0] = {x + width / 2, y			  , width / 2, height / 2};
			draw_rects[1] = {x			  , y + height / 2, width / 2, height / 2};
			draw_rects[2] = {x			  , y			  , width / 2, height / 2};
			draw_rects[3] = {x + width / 2, y + height / 2, width / 2, height / 2};

			mp_render_draw_rects(renderer, draw_rects, total_rects);
			break;
		}
		case DI_mp_render_copy: {
			draw_string(renderer, font, "render_copy", title_x, title_y, string_size, true, true);

			MP_Rect dst_rect = {x, y, width, height};
			mp_set_texture_alpha_mod(debug_texture, 255);
			mp_render_copy(renderer, debug_texture, NULL, &dst_rect);
			break;
		}
		case DI_mp_render_copy_alpha: {
			draw_string(renderer, font, "alpha_mod", title_x, title_y, string_size, true, true);

			MP_Rect dst_rect = {x, y, width, height};
			mp_set_texture_alpha_mod(debug_texture, 100);
			mp_render_copy(renderer, debug_texture, NULL, &dst_rect);
			break;
		}
		case DI_2d_matrix_transformation_rect: {
			draw_string(renderer, font, "2d_rotation", title_x, title_y, string_size, true, true);
			draw_debug_2d_rotation_matrix_rect(renderer, { (float)x + width / 2, (float)y + height / 2 }, &font);
			break;
		}
		case DI_copy_ex: {
			draw_string(renderer, font, "rnd_cpy_ex", title_x, title_y, string_size, true, true);

			MP_Rect temp_rect = { x, y, width, height};
			static float temp_angle = 0;
			if (key_pressed_and_held(KEY_A)) {
				temp_angle += 1;
			} 
			if (key_pressed_and_held(KEY_D)) {
				temp_angle -= 1;
			}
			mp_render_copy_ex(renderer, images[IT_Sun].texture, NULL, &temp_rect, temp_angle, NULL, SDL_FLIP_NONE);
			break;
		}
		default: {
			draw_string(renderer, font, "Error. Image default case hit in draw debug image function", x, y, 1, true, true);
			break;
		}
        }
    }
	mp_set_texture_color_mod(font.image.texture, 255, 255, 255);
	mp_set_texture_alpha_mod(debug_texture, 255);
}
