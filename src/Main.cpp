#include "Renderer.h"
#include "GL_Functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Image {
	int w;
	int h;
	MP_Texture* texture;
};

Image load_image(MP_Renderer* renderer, const char* file_path) {
	Image result = {};

	// Might be necessary for opengl, testing soon
	stbi_set_flip_vertically_on_load(true); 

	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);
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

struct Font {
	int char_width;
	int char_height;

	int bitmap_width;
	int bitmap_height;

	Image image;
};

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
		default: {
			break;
		}
        }
    }
	mp_set_texture_color_mod(font.image.texture, 255, 255, 255);
	mp_set_texture_alpha_mod(debug_texture, 255);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	MP_Renderer* renderer = mp_create_renderer(hInstance);

	Image sun = load_image(renderer, "assets\\sun.png");

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	MP_Rect player = {0, 0, 200, 200};

	Font font_1 = load_font(renderer, "assets\\basic_font.png");

	bool toggle_debug_images = true;
	bool running = true;
	while (running) {
		reset_is_pressed();

		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) {
				running = false;
			}
		}

		int player_speed = 1;

		if (key_pressed_and_held(KEY_W)) {
			player.y += player_speed;
		}
		if (key_pressed_and_held(KEY_S)) {
			player.y -= player_speed;
		}
		if (key_pressed_and_held(KEY_D)) {
			player.x += player_speed;
		}
		if (key_pressed_and_held(KEY_A)) {
			player.x -= player_speed;
		}
		if (key_pressed(VK_OEM_3)) {
			toggle_debug_images = !toggle_debug_images;
		}

		mp_set_render_draw_color(renderer, 155, 155, 155, 255);
		mp_render_clear(renderer);

		mp_set_texture_alpha_mod(sun.texture, 100);
		mp_render_copy(renderer, sun.texture, NULL, &player);

		draw_mp_library_debug_images(renderer, font_1, sun.texture, toggle_debug_images);
		mp_render_present(renderer);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}