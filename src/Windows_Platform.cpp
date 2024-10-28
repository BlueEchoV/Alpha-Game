#include "Windows_Platform.h"

std::unordered_map<WPARAM, Key_State> player_input;

bool key_pressed(WPARAM key) {
	if (player_input[key].is_pressed || player_input[key].is_held_down) {
		return true;
	} else {
		return false;
	}
}

void reset_is_pressed() {
	for (auto& key_state : player_input) {
		key_state.second.is_pressed = false;
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
		OutputDebugStringA("WM_SIZE\n");
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
