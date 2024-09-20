#include <windows.h>

// #include <GLFW/glfw3.h>

#define REF(v) (void)v;

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
		if (window_handle) {
			for(;;) {
				MSG message;
				BOOL message_result = GetMessage(&message, 0, 0, 0);
				if (message_result > 0) {
					TranslateMessage(&message);
					DispatchMessage(&message);
				} else {
					break;
				} 
			}
		} else {
			// TODO: Handle the rare case the window fails
		}
	}

	bool running = true;
	while (running) {

	}

	return 0;
}