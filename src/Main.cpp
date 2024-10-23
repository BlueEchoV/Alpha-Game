#include "Renderer.h"
#include "GL_Functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#if 0
struct Image {
	int w;
	int h;
	const char* pixel_data;
};

Image load_image(const char* file_path) {

	// stbi_load();
}
#endif 

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	MP_Renderer* renderer = mp_create_renderer(hInstance);

	// GLuint my_texture = create_gl_texture("assets\\sun.png");

	bool running = true;
	while (running) {
		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) {
				running = false;
			}
		}

		mp_set_render_draw_color(renderer, 255, 0, 0, 255);
		mp_render_clear(renderer);

		mp_render_present(renderer);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}