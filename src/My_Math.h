#pragma once
#define _USE_MATH_DEFINES
#include <math.h>


struct V2 {
	float x, y;

	V2 &operator*=(float a);
	V2 &operator+=(V2 a);
};

struct V3 {
	float x;
	float y;
	float z;
};

V2 operator*(const float a, V2 b);
V2 operator-(V2 a);
V2 operator+(V2 a, V2 b);
V2 operator-(const V2 vec1, const V2 vec2);

V2 rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float ws_x, float ws_y);

void swap(V2& vec_1, V2& vec_2);
float hypotenuse(float a, float b);
V2 normalize(V2 vec);

float convert_degrees_to_radians(float degrees);
float convert_radians_to_degrees(float radians);

V2 calculate_normalized_origin_to_target_velocity(V2 target, V2 origin);
float calculate_facing_direction(V2 vec);
