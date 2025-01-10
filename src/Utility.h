#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <stb_image.h>
#include <assert.h>
#include <math.h>

#define M_PI 3.141592653589793238462643383279502884197

#define REF(v) (void)v;

/**********************************************
 *
 * Defer
 *
 ***************/

template <typename T>
struct ExitScope
{
	T lambda;
	ExitScope(T lambda) : lambda(lambda) { }
	~ExitScope() { lambda(); }
	// NO_COPY(ExitScope);
};

struct ExitScopeHelp
{
	template<typename T>
	ExitScope<T> operator+(T t) { return t; }
};

#define _SG_CONCAT(a, b) a ## b
#define SG_CONCAT(a, b) _SG_CONCAT(a, b)
#define DEFER auto SG_CONCAT(defer__, __LINE__) = ExitScopeHelp() + [&]()


struct V2_F {
	float x;
	float y;
};

struct V3_F {
	float x;
	float y;
	float z;
};

struct Color_3F {
	float r;
	float g;
	float b;
};

struct Color_4F {
	float r;
	float g;
	float b;
	float a;
};

struct Color_RGBA8 {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

void log(const char* format, ...);
void my_mem_copy(const void* src, void* dst, size_t n);
V2_F rotate_point_based_off_angle(float angle_in_degrees, float center_x, float center_y, float ws_x, float ws_y);
