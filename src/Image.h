#pragma once
#include "Renderer.h"
#include <unordered_map>
#include <string>

struct Image {
	int w;
	int h;

	// This is the distance from the center of the image to 
	// the farthest non transparent pixel in the sprite.
	float sprite_radius;

	MP_Texture* texture;
};

enum Tile_Type {
	TT_Rock,
	TT_Grass,
	TT_Water,
	TT_Total
};

enum Image_Type {
	IT_Sun,
	IT_Dummy_Player,
	IT_Player_Rugged_Male,
	IT_Enemy_Clothed_Zombie,
	IT_Top_Down_Player_Demo,
	IT_Arrow_1,
	IT_Dummy_Tile_32x32,
	IT_Rock_32x32,
	IT_Grass_32x32,
	IT_Water_32x32
};

Image load_image(const char* file_path);
Image load_image(const char* file_path, int access, bool use_linear_filtering);

extern std::unordered_map<std::string , Image> images;
void load_images();
Image* get_image(std::string_view image_name);

