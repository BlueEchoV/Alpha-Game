#pragma once
#include "Utility.h"

struct Renderer {
};

struct Vertex_2D {
	V2 pos;
};

void get_window_size(HWND window, int& w, int& h);
void init_open_gl(HWND window);
void create_renderer();

void draw_triangle();
