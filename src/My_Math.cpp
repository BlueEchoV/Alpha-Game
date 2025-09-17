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
V2 rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float x, float y) {
	V2 result = {};

	float angle_in_radians = angle_in_degrees * ((float)M_PI / 180.0f);

	// Translate the world space x and y so that the center is 0,0
	double translated_old_x = x - center_x;
	double translated_old_y = y - center_y;

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
	// 1-2 cpu cycles
	int x = 4;
	int y = 2;
	// add and sub take 1-2 cycles
	x = y + x;
	x = y - x;
	x = y * x;
	// sqrt requires 20-50 more cycles or more
	double z = sqrt(y);
	z++;

	result = (float)sqrt(fabs((a * a) + (b * b)));

	return result;
}

V2 normalize(V2 vec) {
	V2 result = {};

	// Calculate length
	float length = hypotenuse(vec.x, vec.y);

	// Normalize the vector
	if (vec.x != 0) {
		result.x = vec.x / length;
	}
	if (vec.y != 0) {
		result.y = vec.y / length;
	}

	return result;
}

void clamp(float& clamp_value, float min, float max) {
	if (clamp_value < min) {
		clamp_value = min;
	}
	else if (clamp_value > max) {
		clamp_value = max;
	}
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

V2 calculate_origin_to_target_velocity(V2 target, V2 origin) {
	V2 result = {};

	result = target - origin;

	return result;
}

V2 calculate_normalized_origin_to_target_velocity(V2 target, V2 origin) {
	return normalize(calculate_origin_to_target_velocity(target, origin));
}

float calculate_facing_direction(V2 vec) {
    float len = sqrt(vec.x * vec.x + vec.y * vec.y);
    if (len == 0.0f) return 0.0f;  // Default to north

    // Standard atan2(y, x) for counterclockwise from east (0-360° after normalization)
    float angle_rad = atan2(vec.y, vec.x);
    float angle = angle_rad * (180.0f / 3.14159265359f);  // Convert to degrees
    if (angle < 0.0f) angle += 360.0f;

    // Shift to clockwise from north: 90° - angle (mod 360)
    angle = fmod(90.0f - angle + 360.0f, 360.0f);

    return angle;
}

float calculate_distance_between(V2 vec_1, V2 vec_2) {
	float result = {};

	V2 direction_vector = vec_1 - vec_2;
	result = hypotenuse(direction_vector.x, direction_vector.y);

	return result;
}

float lerp(float a_starting_val, float b_ending_val, float t_where) {
	return a_starting_val + (t_where * (b_ending_val - a_starting_val));
}
