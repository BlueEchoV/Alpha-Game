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

// Retrieves the current mouse position in window coordinates, adjusted to a bottom-left origin.
// This function uses Windows API calls to get the cursor position relative to the client area of the window.
// Note: The result is in pixel units relative to the window's client area, with y increasing upwards from the bottom.
V2 get_mouse_position(HWND hwnd) {
    V2 result = {};  // Initialize result to (0, 0) by default.
    POINT ptr;       // Structure to hold the cursor position in screen coordinates.
    if (GetCursorPos(&ptr)) {  // Fetch the absolute cursor position on the screen.
        RECT clientRect;       // Structure to hold the dimensions of the window's client area.
        GetClientRect(hwnd, &clientRect);  // Retrieve the client area's bounding rectangle.
        if (ScreenToClient(hwnd, &ptr)) {  // Convert screen coordinates to client-relative coordinates.
            // Account for potential scaling issues (e.g., DPI awareness), though no explicit scaling is applied here.
            result.x = (float)ptr.x;  // Set x to the client-relative horizontal position.
            result.y = (float)ptr.y;  // Set y to the client-relative vertical position (top-down by default).
            
            // Convert to bottom-left coordinate system: Flip y so it increases upwards from the bottom.
            result.y = (float)clientRect.bottom - result.y;
        }
    } else {
        // Log an error if GetCursorPos fails
        log("Error: GetCursorPos failed.");
        assert(false);  // Halt execution in debug mode to highlight the failure.
    }
    return result;  // Return the adjusted mouse position.
}

// Returns mouse position in playground coordinates (logical game space).
// This function transforms the raw window mouse position into the game's logical coordinate system,
// accounting for viewport offsets and scaling due to letterboxing or resizing.
// Returns a V2 with x/y in [0, playground_area_w] and [0, playground_area_h] if within the viewport;
// otherwise, returns {-1.0f, -1.0f} to indicate the mouse is outside the active viewport (e.g., in letterbox areas).
// Assumes the playground coordinates use a bottom-left origin, matching the adjusted window coordinates.
V2 get_playground_mouse_position(HWND hwnd) {
    // Retrieve the mouse position in window coordinates (bottom-left origin).
    V2 window_mouse = get_mouse_position(hwnd);
    
    // Check if the mouse position falls within the active viewport's boundaries in window space.
    // This prevents processing positions in inactive areas like black bars.
    if (window_mouse.x < Globals::active_viewport_x ||
        window_mouse.x > Globals::active_viewport_x + Globals::active_viewport_w ||
        window_mouse.y < Globals::active_viewport_y ||
        window_mouse.y > Globals::active_viewport_y + Globals::active_viewport_h) {
        return {-1.0f, -1.0f};  // Return invalid position indicator for positions outside the viewport.
    }
    
    // Transform to playground space: 
    // First, subtract the viewport's top-left offset to make it relative to the rendering area.
    // Then, divide by the scale factors to map from physical pixels to logical units,
    // effectively reversing the viewport's scaling.
    float playground_x = (window_mouse.x - Globals::active_viewport_x) / Globals::active_viewport_scale_x;
    float playground_y = (window_mouse.y - Globals::active_viewport_y) / Globals::active_viewport_scale_y;
    
    // Return the transformed position in logical playground coordinates.
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
