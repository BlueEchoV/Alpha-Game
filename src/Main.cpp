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

	Renderer* renderer = create_renderer(hInstance);

	init_texture();

	GLuint my_texture = create_gl_texture("assets\\sun.png");

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

		render_clear();

		draw_texture(my_texture);

		render_present(renderer);

	}

	// ReleaseDC(window_handle, window_dc);
	return 0;
}