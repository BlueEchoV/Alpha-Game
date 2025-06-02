#pragma once
#include "Image.h"

enum Facing_Direction {
	FD_Right,
	FD_Left
};

enum Animation_Play_Speed {
	APS_Slow,
	APS_Fast,
	APS_Speed_Based,
	APS_No_Animation
};

enum Animation_State {
	AS_Idle,
	AS_Walking,
	AS_Running,
	AS_Attacking,
	AS_Death,
	AS_No_Animation
};

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
	std::vector<Sprite> sprites;
};

struct Animation_Tracker {
	// entity name is the sprite sheet category
	std::string entity_name;
	std::string selected_sprite_sheet;

	Animation_State as;
	Animation_Play_Speed aps;
	Facing_Direction fd; 
	int current_frame_index;
	float current_frame_time;
	bool loops = true;
};

Sprite create_sprite(Image image, MP_Rect src_rect);

extern std::unordered_map<std::string, Sprite_Sheet> sprite_sheet_map;

Sprite_Sheet* get_sprite_sheet(std::string name);
Sprite_Sheet create_animation_sprite_sheet(std::string full_file_path, int rows, int columns);
Animation_Tracker create_animation_tracker(std::string entity_name, Animation_State starting_as, bool loops);
void change_animation(Animation_Tracker* at, std::string entity_name, Animation_State new_as,
	Facing_Direction facing_direction, Animation_Play_Speed animation_play_speed);
void update_animation_tracker(Animation_Tracker* at, float delta_time, float speed);
void update_animation_tracker(Animation_Tracker* at, float delta_time);
void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst, float angle);

void load_sprite_sheets();
void load_sprite_sheet_data_csv(CSV_Data* data);
