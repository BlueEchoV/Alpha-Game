#pragma once
#include "Utility.h"
#include <unordered_map>

typedef unsigned int GLuint;

struct Renderer {
	HDC window_dc;
};

struct Vertex_2D {
	V3_F pos;
	Color_3F color;
	V2_F texture_coor;
};

void get_window_size(HWND window, int& w, int& h);
HDC init_open_gl(HWND window);
void load_shaders();
Renderer* create_renderer(HINSTANCE hInstance);
void render_clear();
void render_present(Renderer* renderer);
GLuint create_gl_texture(const char* file_path);

void init_texture();
void draw_texture(GLuint texture);
