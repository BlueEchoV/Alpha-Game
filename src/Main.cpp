#include <windows.h>

// This brings in all the openGL functions we might need so 
// we can talk to OpenGL
#include <gl/GL.h>

#define REF(v) (void)v;

// TODO: Handle input in a more efficient way (unordered map)

void init_open_gl(HWND window) {
	HDC window_dc = GetDC(window);

	// IMPORTANT: There is a lot of randomness to this. Don't be discouraged if 
	// you don't remember it all.

	// PIXEL FORMAT DESCRIPTOR: The Pixel Format Descriptor (PFD) is a structure 
	// used in Windows-based OpenGL applications to define the properties and 
	// Windows content will be drawn. This structure provides essential information 
	// about how pixels are handled in the rendering context, including color depth, 
	// buffer types, transparency, and other graphical features like depth and stencil 
	// buffers.
	
	// NOTE: The pixel format descriptor is a negotiation we don't 
	// actually fill out a format descriptor, we find one
	PIXELFORMATDESCRIPTOR desired_pixel_format = {};
	desired_pixel_format.nSize = sizeof(desired_pixel_format);
	desired_pixel_format.nVersion = 1;
	desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	desired_pixel_format.cColorBits = 32;
	desired_pixel_format.cAlphaBits = 8;
	desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

	// NOTE: We ask for the pixel format the the one above matches the closes
	int suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);
	PIXELFORMATDESCRIPTOR suggested_pixel_format;
	// NOTE: Sets the members of the PIXELFORMATDESCRIPTOR structure pointed to by ppfd 
	// with that pixel format data.
	DescribePixelFormat(
		window_dc,
		suggested_pixel_format_index,
		sizeof(suggested_pixel_format),
		&suggested_pixel_format
	);
	SetPixelFormat(window_dc, suggested_pixel_format_index, &suggested_pixel_format);

	HGLRC open_gl_rc = wglCreateContext(window_dc);
	if (wglMakeCurrent(window_dc, open_gl_rc)) {
		// Success
	} else {
		// failure
	}
	ReleaseDC(window, window_dc);
}

void* GetAnyGLFuncAddress(const char *name) {
  void *p = (void *)wglGetProcAddress(name);
  if(p == 0 ||
    (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
    (p == (void*)-1) )
  {
    HMODULE module = LoadLibraryA("opengl32.dll");
    p = (void *)GetProcAddress(module, name);
  }

  return p;
}

void load_open_gl_functions() {

}

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