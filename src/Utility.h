#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <stb_image.h>
#include <assert.h>
#include <math.h>

#include "Globals.h"
#include "My_Math.h"

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

struct Color_RGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct Color_RGBA {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

void log(const char* format, ...);
bool my_mem_compare(const void* src, const void* dst, size_t n);
void my_mem_copy(const void* src, void* dst, size_t n);

enum Type {
	T_Int,
	T_Float
};

#define FIELD(struct_type, variable_type, variable_name) { variable_type, offsetof(struct_type, variable_name), #variable_name};

// For each column 
struct Type_Descriptor {
	Type variable_type;
	int offset;
	std::string variable_name;
};

struct CSV_Data {
	std::string file_name;
	FILE* file;
};
