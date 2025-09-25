#pragma once
#include "Image.h"
#include <optional>  // For std::optional<V2>

enum Facing_Direction {
	FD_NONE, // Optional
	FD_N,   // 0
	FD_NNE, // 22.5
	FD_NE,  // 45
	FD_ENE, // 67.5
	FD_E,   // 90
	FD_ESE, // 112.5
	FD_SE,  // 135
	FD_SSE, // 157.5
	FD_S,   // 180
	FD_SSW, // 202.5
	FD_SW,  // 225
	FD_WSW, // 247.5
	FD_W,   // 270
	FD_WNW, // 292.5
	FD_NW,  // 315
	FD_NNW, // 337.5
};

enum Animation_State {
	AS_Idle,
	AS_Walking,
	AS_Running,
	AS_Attacking,
	AS_Dying,
	AS_No_Animation
};

enum Animation_Play_Speed {
	APS_Slow,
	APS_Fast,
	APS_Speed_Based,
	APS_No_Animation
};

enum Animation_Mode {
    AM_Animate_Looping,
    AM_Animate_Looping_Reversed,
    AM_Animate_Once,
    AM_Static_First_Frame,
    AM_Static_Last_Frame,
    AM_No_Animation
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
	int columns;
	int rows;
	std::vector<Sprite> sprites;
};

enum Animation_Tracker_Type {
	ATT_Direction_2,
	ATT_Direction_8,
	ATT_Direction_8_Legs,
	// For Player
	ATT_Direction_16,
	ATT_Direction_16_Torso,
	ATT_Direction_8_Atlas
};

struct Animation_Tracker {
	Animation_Tracker_Type att;
	// entity name is the sprite sheet category
	std::string entity_name;
	std::string selected_sprite_sheet;

	Animation_State as;
	Animation_Mode mode;
	Animation_Play_Speed aps;
	Facing_Direction fd; 
	bool flip_horizontally = false;
	bool force_play_whole_animation = false;
	// Total frames is the number of columns
	int current_frame_index;
	float current_frame_time;
	bool loops = true;
	V2 last_velocity = { 0, 0 };
};

extern std::unordered_map<std::string, Sprite_Sheet> sprite_sheet_map;
Sprite_Sheet* get_sprite_sheet(std::string name);
Sprite_Sheet create_animation_sprite_sheet(std::string full_file_path, int rows, int columns);

Animation_Tracker create_animation_tracker(Animation_Tracker_Type att, std::string_view entity_name,
	Animation_State starting_as, Animation_Play_Speed starting_aps, Animation_Mode starting_mode, bool loops);
void update_animation_tracker(Animation_Tracker* at, float delta_time, float speed_based);
void change_animation_tracker(Animation_Tracker* at, const std::string& entity_name, Animation_State new_as, Animation_Play_Speed aps, Animation_Mode mode,
	bool flip_horizontally, std::optional<V2> velocity_opt);
void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst, float angle, uint8_t alpha_mod);
void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst, float angle);
void draw_animation_tracker_outlined(Animation_Tracker* at, MP_Rect dst, float angle, Color_Type ct, float outline_thickness);

void load_sprite_sheets();
void load_sprite_sheet_data_csv(CSV_Data* data); 