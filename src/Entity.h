#pragma once
#include "Utility.h"
#include "renderer.h"
#include "Image.h"

struct MP_Renderer;

struct Player {
	Image* image;

	// World Space
	V2 pos;

	int w, h;

	// int health;
	// int damage;
	int speed;
};

struct Arrow {
	V2 pos_ws;
	V2 vel;
	int speed;
	int w, h;
	float angle;

	Image* image;
};

V2 convert_cs_to_ws(V2 entity_pos, V2 camera_pos);
V2 convert_cs_to_ws(int entity_x, int entity_y, int camera_x, int camera_y);
V2 convert_ws_to_cs(V2 entity_pos, V2 camera_pos);
V2 convert_ws_to_cs(int entity_x, int entity_y, int camera_x, int camera_y);

Player create_player(Image* image, int player_speed);

Arrow create_arrow(Image* image, V2 pos, V2 vel, int width, int height, int speed);
void update_arrow(Arrow& arrow, float delta_time);
void draw_arrow(int camera_pos_x, int camera_pos_y, Arrow& arrow);

// void spawn_entity(V2_F pos);
// void update_entity();
// void draw_entity();
