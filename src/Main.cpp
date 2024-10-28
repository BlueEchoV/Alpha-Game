#include "Renderer.h"
#include "GL_Functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Image {
	int w;
	int h;
	// const char* pixel_data;
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


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	MP_Renderer* renderer = mp_create_renderer(hInstance);

	Image sun = load_image(renderer, "assets\\sun.png");

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	MP_Rect player = {0, 0, 200, 200};

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

		if (key_pressed(KEY_W)) {
			player.y += player_speed;
		}
		if (key_pressed(KEY_S)) {
			player.y -= player_speed;
		}
		if (key_pressed(KEY_D)) {
			player.x += player_speed;
		}
		if (key_pressed(KEY_A)) {
			player.x -= player_speed;
		}

		mp_set_render_draw_color(renderer, 155, 155, 155, 255);
		mp_render_clear(renderer);

		mp_render_copy(renderer, sun.texture, NULL, &player);

		mp_render_present(renderer);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}