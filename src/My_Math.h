#pragma once
#include <math.h> 

#define M_PI 3.141592653589793238462643383279502884197

struct V2_F {
	float x;
	float y;
};

struct V2_I {
	int x;
	int y;
};

struct V3_F {
	float x;
	float y;
	float z;
};

V2_F operator-(const V2_F vec1, const V2_F vec2);

V2_F rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float ws_x, float ws_y);

void swap(V2_F& vec_1, V2_F& vec_2);
float hypotenuse(float a, float b);
