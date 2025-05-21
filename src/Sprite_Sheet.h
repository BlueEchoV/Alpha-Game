#pragma once
#include "Image.h"

struct Sprite {
	Image* image;
};

// Animation Sprite Sheet?
struct Sprite_Sheet {
	int rows;
	int columns;
	std::vector<Sprite> sprites;
};

Sprite_Sheet create_sprite_sheet(int rows, int columns, const char* file_name);
