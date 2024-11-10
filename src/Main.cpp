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
	Image image;
};

const int bitmap_width = 18;
const int bitmap_height = 7;
Font load_font(MP_Renderer* renderer, const char* file_path) {
	Font font = {};;

	font.image = load_image(renderer, file_path);

	font.char_width = font.image.w / bitmap_width;
	font.char_height = font.image.h / bitmap_height;

	return font;
}

enum Debug_Image {
    DI_mp_render_fill_rect,
    DI_mp_render_fill_rects,
	DI_mp_render_draw_line,
	DI_mp_render_draw_lines,
	DI_mp_render_draw_point,
	DI_mp_render_draw_points,
    DI_Count
};

void draw_mp_library_debug_images(MP_Renderer* renderer, bool toggle_debug_images) {
	if (!toggle_debug_images) { 
		return;
	}

    int width = 100;
    int height = 100;

    int starting_x = 50;
    int starting_y = renderer->window_height - (50 + height);

    int offset_x = 50 + width;
    int offset_y = -50 - height;

    mp_set_render_draw_color(renderer, 255, 0, 0, 255);

    int images_per_row = 5;

    for (int i = 0; i < DI_Count; i++) {
        int x = starting_x + (i % images_per_row) * offset_x;
        int y = starting_y + (i / images_per_row) * offset_y; 

        MP_Rect rect = {x, y, width, height};

        switch ((Debug_Image)i) {
		case DI_mp_render_fill_rect: {
			mp_set_render_draw_color(renderer, 255, 0, 0, 255);
			mp_render_fill_rect(renderer, &rect);
			break;
		}
		case DI_mp_render_fill_rects: {
			mp_set_render_draw_color(renderer, 0, 255, 0, 255); 
			MP_Rect bottom_left_and_top_right[2] = {
				{x		      , y             , width / 2, height / 2}, 
				{x + width / 2, y + height / 2, width / 2, height / 2}
			};
			mp_render_fill_rects(renderer, bottom_left_and_top_right, ARRAYSIZE(bottom_left_and_top_right));

			mp_set_render_draw_color(renderer, 0, 0, 255, 255); 
			MP_Rect bottom_right_and_top_left[2] = {
				{x + width / 2, y			  , width / 2, height / 2}, 
				{x            , y + height / 2, width / 2, height / 2} 
			};
			mp_render_fill_rects(renderer, bottom_right_and_top_left, ARRAYSIZE(bottom_right_and_top_left));
			break;
		}
		case DI_mp_render_draw_line: {
			mp_set_render_draw_color(renderer, 255, 0, 0, 255);
			mp_render_draw_line(renderer, x, y, x + width, y);

			mp_set_render_draw_color(renderer, 0, 255, 0, 255);
			mp_render_draw_line(renderer, x, y + height / 2, x + width, y + height / 2);

			mp_set_render_draw_color(renderer, 0, 0, 255, 255);
			mp_render_draw_line(renderer, x, y + height, x + width, y + height);

			mp_set_render_draw_color(renderer, 255, 0, 0, 255);
			mp_render_draw_line(renderer, x, y, x, y + height);

			mp_set_render_draw_color(renderer, 0, 255, 0, 255);
			mp_render_draw_line(renderer, x + width / 2, y, x + width / 2, y + height);

			mp_set_render_draw_color(renderer, 0, 0, 255, 255);
			mp_render_draw_line(renderer, x + width, y, x + width, y + height);
			break;
		}
		case DI_mp_render_draw_lines: {
			mp_set_render_draw_color(renderer, 255, 255, 0, 255);

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
			mp_set_render_draw_color(renderer, 255, 0, 0, 255);
			mp_render_draw_point(renderer, x		, y			);
			mp_render_draw_point(renderer, x + width, y			);
			mp_render_draw_point(renderer, x		, y + height);
			mp_render_draw_point(renderer, x + width, y + height);
			break;
		}
		case DI_mp_render_draw_points: {
			mp_set_render_draw_color(renderer, 0, 255, 0, 255);
			const int total_points = 4;
			MP_Point points[total_points];
			points[0] = { x		   , y          };
			points[1] = { x + width, y          };
			points[2] = { x		   , y + height };
			points[3] = { x + width, y + height };
			mp_render_draw_points(renderer, points, total_points);
			break;
		}
		default: {
			break;
		}
        }
    }
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

		mp_render_copy(renderer, sun.texture, NULL, &player);

		draw_mp_library_debug_images(renderer, toggle_debug_images);
		mp_render_present(renderer);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}