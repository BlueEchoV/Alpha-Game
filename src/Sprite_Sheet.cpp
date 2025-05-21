#include "Sprite_Sheet.h"

Sprite create_sprite() {
	Sprite result = {};

	return result;
}

Sprite_Sheet create_animation_sprite_sheet(int rows, int columns, const char* file_name) {
	Sprite_Sheet result = {};
	REF(file_name);

	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < columns; y++) {

		}
	}

	return result;
}

Sprite_Sheet create_variable_images_sprite_sheet(int rows, int columns, const char* file_name) {
	Sprite_Sheet result = {};
	REF(file_name);
	REF(rows);
	REF(columns);

	return result;
}
