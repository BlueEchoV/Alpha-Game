#pragma once
#include "Image.h"

struct Sprite {
	Image* image;
	MP_Rect src_rect;
};

// Animation Sprite Sheet?
struct Sprite_Sheet {
	float default_frame_speed;
	std::vector<Sprite> sprites;
};

struct Animation_Tracker {
	std::string selected_sprite_sheet;

	int current_frame_index;
	float current_frame_time;
};

Sprite create_sprite(Image* image, MP_Rect src_rect);

extern std::unordered_map<std::string, Sprite_Sheet> sprite_sheet_map;

void load_sprite_sheets();
Sprite_Sheet* get_sprite_sheet(std::string name);
Sprite_Sheet create_animation_sprite_sheet(std::string image_name, float default_frame_speed, int rows, int columns);
Animation_Tracker create_animation_tracker(std::string selected_sprite_sheet);
void update_animation_tracker(Animation_Tracker* at, float delta_time);
void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst);
