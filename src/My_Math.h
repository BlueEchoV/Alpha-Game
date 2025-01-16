#pragma once
#include <math.h> 

#define M_PI 3.141592653589793238462643383279502884197

struct V2 {
	float x;
	float y;
};

struct V3 {
	float x;
	float y;
	float z;
};

V2 operator-(const V2 vec1, const V2 vec2);

V2 rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float ws_x, float ws_y);

void swap(V2& vec_1, V2& vec_2);
float hypotenuse(float a, float b);
