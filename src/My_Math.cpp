#include "My_Math.h"

V2 operator*(const float a, V2 b) {
	V2 result = {};

	result.x = a * b.x;
	result.y = a * b.y;

	return result;
}

V2& V2::operator*=(float a) {
	*this = a * *this;
	return *this;
}

V2 operator-(V2 a) {
	V2 result = {};

	result.x = -a.x;
	result.y = -a.y;

	return result;
}

V2 operator+(V2 a, V2 b) {
	V2 result = {};

	result.x = a.x + b.x;
	result.y = a.y + b.y;

	return result;
}

V2& V2::operator+=(V2 a) {
	*this = a + *this;
	return *this;
}

V2 operator-(const V2 vec1, const V2 vec2) {
	V2 result = {};

	result.x = vec1.x - vec2.x;
	result.y = vec1.y - vec2.y;

	return result;
}

// Rotates a point based off an angle
V2 rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float ws_x, float ws_y) {
	V2 result = {};

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

float hypotenuse(float a, float b) {
	float result = {};

	result = (float)sqrt(fabs((a * a) + (b * b)));

	return result;
}

V2 normalize(V2 vec) {
	V2 result = {};

	// Calculate length
	float length = hypotenuse(vec.x, vec.y);

	// Normalize the vector
	result.x = vec.x / length;
	result.y = vec.y / length;

	return result;
}

void swap(V2& vec_1, V2& vec_2) {
	V2 temp = vec_1;
	vec_1 = vec_2;
	vec_2 = temp;
}

float convert_degrees_to_radians(float degrees) {
	return degrees * ((float)M_PI / 180.0f);
}

float convert_radians_to_degrees(float radians) {
	return radians * (180.0f / (float)M_PI);
}
