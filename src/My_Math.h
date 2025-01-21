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

float convert_degrees_to_radians(float degrees);
