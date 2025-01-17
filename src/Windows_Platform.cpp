#include "Windows_Platform.h"

std::unordered_map<WPARAM, Key_State> player_input;

bool key_pressed(WPARAM key) {
	if (player_input[key].is_pressed) {
		return true;
	}
	return false;
}

bool key_pressed_and_held(WPARAM key) {
	if (player_input[key].is_pressed || player_input[key].is_held_down) {
		return true;
	}
	return false;
}

void reset_is_pressed() {
	for (auto& key_state : player_input) {
		key_state.second.is_pressed = false;
	}
}

void get_window_size(HWND window, int& w, int& h) {
	RECT rect = {};
	if (GetClientRect(window, &rect) != 0) {
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
	} else {
		w = 0;
		h = 0;
		log("Window width and height are 0");
	}
}

LRESULT wind_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	// Returning 0 means we processed the message
	LRESULT result = 0;

	switch(message) {
	case WM_KEYDOWN: {
		player_input[wparam].is_pressed = true;
		player_input[wparam].is_held_down = true;
		break;
	}
	case WM_KEYUP: {
		player_input[wparam].is_pressed = false;
		player_input[wparam].is_held_down = false;
		break;
	}
	case WM_SIZE: {
		switch (wparam) {
		case SIZE_RESTORED:
			// The window has been resized, but neither the 
			break;
		case SIZE_MINIMIZED:
			// Handle when the window is minimized
			break;
		case SIZE_MAXIMIZED:
			// Handle when the window is maximized
			break;
		default:
			break;
		}
		break;
	}
	case WM_ACTIVATEAPP: {
		OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;
	} 	
	case WM_CLOSE: {
		log("Destroying window");
		DestroyWindow(window);
		break; 
	} 
	case WM_DESTROY: {
		PostQuitMessage(0);
		OutputDebugStringA("WM_DESTROY\n");
		break;
	}
	default: {
		result = DefWindowProc(window, message, wparam, lparam);
		break;
	}
	}

	return result;
}
