#pragma once
#include "Image.h"

struct Sprite {
	Image image;
	MP_Rect src_rect;
};

struct Sprite_Sheet_Data {
	// For constructing the file path
	std::string root;
	std::string sprite_sheet_name;
	std::string ext;

	int rows;
	int columns;
};

// Animation Sprite Sheet?
struct Sprite_Sheet {
	float default_frame_speed_seconds;
	std::vector<Sprite> sprites;
};

struct Animation_Tracker {
	std::string selected_sprite_sheet;

	bool flip;
	int current_frame_index;
	float current_frame_time;
};

Sprite create_sprite(Image image, MP_Rect src_rect);

extern std::unordered_map<std::string, Sprite_Sheet> sprite_sheet_map;

Sprite_Sheet* get_sprite_sheet(std::string name);
Sprite_Sheet create_animation_sprite_sheet(std::string full_file_path, float default_frame_speed_seconds, int rows, int columns);
Animation_Tracker create_animation_tracker(std::string selected_sprite_sheet);
void change_animation(Animation_Tracker* at, std::string new_selected_sprite_sheet, bool flip);
void update_animation_tracker(Animation_Tracker* at, float delta_time);
void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst);

void load_sprite_sheets();
void load_sprite_sheet_data_csv(CSV_Data* data);
