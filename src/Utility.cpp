#include "Utility.h" 

void log(const char* format, ...) {
	char buffer[256];
	va_list arguments;

	va_start(arguments, format);
	vsnprintf(buffer, 256, format, arguments);
	va_end(arguments);

	OutputDebugString(buffer);
	OutputDebugString("\n");
}

void my_mem_copy(const void* src, void* dst, size_t n) {
	const char* src_byte = (const char*)src;
	char* dst_byte = (char*)dst;

	for (int i = 0; i < n; i++) {
		// Copy byte to byte (char is 1 byte)
		dst_byte[i] = src_byte[i];
	}
}

// Rotates a point based off an angle
V2_F rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float ws_x, float ws_y) {
	V2_F result = {};

	float angle_in_radians = angle_in_degrees * ((float)M_PI / 180.0f);

	// Translate the world space x and y so that the center is 0,0
	double translated_old_x = ws_x - center_x;
	double translated_old_y = ws_y - center_y;

	// Rotation matrix
	float translated_new_x = (float)(translated_old_x * cos(angle_in_radians) - translated_old_y * sin(angle_in_radians));
	float translated_new_y = (float)(translated_old_x * sin(angle_in_radians) + translated_old_y * cos(angle_in_radians));

	// Translate back to world space
	result.x = translated_new_x + center_x;
	result.y = translated_new_y + center_y;

	return result;
}
