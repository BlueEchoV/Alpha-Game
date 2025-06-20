#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <stb_image.h>
#include <assert.h>
#include <math.h>
#include <span>
#include <unordered_map>

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

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

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

enum Color_Type {
	CT_White,
	CT_Black,
	CT_Red,
	CT_Red_Wine,
	CT_Green,
	CT_Blue,
	CT_Orange,
	CT_Dark_Yellow,
	CT_Dark_Blue,
	CT_Dark_Green,
	CT_Dark_Grey,
	CT_Total
};

void log(const char* format, ...);
bool my_mem_compare(const void* src, const void* dst, size_t n);
void my_mem_copy(const void* src, void* dst, size_t n);

enum Variable_Type {
	VT_Int,
	VT_Float, 
	VT_String
};

// For each column 
struct Type_Descriptor {
	Variable_Type variable_type;
	int offset;
	std::string variable_name;
};

// The #name part is needed for the FieldDescriptor’s name field, which expects a string (const char*) to 
// store the field’s name (e.g., "age") for metadata. Without #name, you’d pass the 
// raw identifier age, which would cause a compilation error since age isn’t a string.
#define FIELD(struct_type, variable_type, variable_name) { variable_type, static_cast<int>(offsetof(struct_type, variable_name)), #variable_name }

struct CSV_Data {
	std::string file_name;
	int total_rows;
	FILE* file;
};

CSV_Data create_open_csv_data(std::string file_name);
bool open_csv_data_file(CSV_Data* data);
bool close_csv_data_file(CSV_Data* data);
void load_csv_data_file(CSV_Data* data, char* destination, std::span<Type_Descriptor> type_descriptors, size_t stride);


