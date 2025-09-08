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

// In camera space, NOT PLAYGROUND SPACE
V2 get_mouse_position(HWND hwnd) {
	V2 result = {};

	POINT ptr;
	if (GetCursorPos(&ptr)) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        if (ScreenToClient(hwnd, &ptr)) {
            // Account for potential scaling issues
            result.x = (float)ptr.x;
            result.y = (float)ptr.y;
            
            // Convert to bottom-left coordinate system
            result.y = (float)clientRect.bottom - result.y;
        }	
	} else {
		log("Error: GetCursorPos failed.");
		assert(false);
	}
	return result;
}

// Returns mouse position in playground coordinates (logical game space).
// Returns a V2 with x/y in [0, playground_area_w] and [0, playground_area_h] if within the viewport;
// otherwise, returns {-1, -1} to indicate the mouse is outside the active viewport (e.g., in letterbox areas).
V2 get_playground_mouse_position(HWND hwnd) {
    V2 window_mouse = get_mouse_position(hwnd);
    
    // Check if mouse is within the active viewport bounds.
    if (window_mouse.x < Globals::active_viewport_x ||
        window_mouse.x > Globals::active_viewport_x + Globals::active_viewport_w ||
        window_mouse.y < Globals::active_viewport_y ||
        window_mouse.y > Globals::active_viewport_y + Globals::active_viewport_h) {
        return {-1.0f, -1.0f};  // Invalid position (outside viewport).
    }
    
    // Transform to playground space: subtract offset, then scale inversely.
    float playground_x = (window_mouse.x - Globals::active_viewport_x) / Globals::active_viewport_scale_x;
    float playground_y = (window_mouse.y - Globals::active_viewport_y) / Globals::active_viewport_scale_y;
    
    return {playground_x, playground_y};
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
	case WM_LBUTTONDOWN:
		player_input[VK_LBUTTON].is_pressed = true;
		player_input[VK_LBUTTON].is_held_down = true;
		break;

	case WM_LBUTTONUP:
		player_input[VK_LBUTTON].is_held_down = false;
		break;
	case WM_SIZE: {
		// SIZE_MINIMIZED is for when the application is minimized 
		// to the taskbar
		// lparam contains the size of the new area. Only update 
		// if it is greater than 0.
		if (wparam != SIZE_MINIMIZED && HIWORD(lparam) > 0) {
			Globals::client_area_w = LOWORD(lparam);
			Globals::client_area_h = HIWORD(lparam);
			Globals::viewport_needs_update = true;
		}
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
