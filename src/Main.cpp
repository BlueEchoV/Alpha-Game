#include "Renderer.h"

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

LRESULT wind_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	// Returning 0 means we processed the message
	LRESULT result = 0;

	switch(message) {
	case WM_SIZE: {
		OutputDebugStringA("WM_SIZE\n");
		break;
	}
	case WM_DESTROY: {
		OutputDebugStringA("WM_DESTROY\n");
		break;
	}
	case WM_CLOSE: {
		OutputDebugStringA("WM_CLOSE\n");
		break;
	}
	case WM_ACTIVATEAPP: {
		OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;
	}
	case WM_PAINT: {

		break;
	}
	default: {
		result = DefWindowProc(window, message, wparam, lparam);
		break;
	}
	}

	return result;
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	REF(hPrevInstance);
	REF(lpCmdLine);
	REF(nShowCmd);

	WNDCLASSA wnd_class = {};

	wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wnd_class.lpfnWndProc = wind_proc;
	wnd_class.hInstance = hInstance;
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd_class.lpszClassName = "Alpha Game";

	HWND window_handle = {};
	if(RegisterClassA(&wnd_class)) {
		window_handle = CreateWindowExA(
			0,
			wnd_class.lpszClassName,	
			"Alpha Game",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			hInstance,
			0);
	}

	init_open_gl(window_handle);

	create_renderer();

	draw_triangle();

	bool running = true;
	while (running) {
		if (window_handle) {
			MSG message;
			while (PeekMessage(&message, window_handle, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		} else {
			// TODO: Handle the rare case the window fails
		}
	}

	return 0;
}