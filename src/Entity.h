#pragma once
#include "Utility.h"
#include "renderer.h"
#include "Image.h"

struct MP_Renderer;

// Colliders
struct Collider {
	float rad;
	// This is offset based off the position of the rigid body
	V2 pos_ls;
};

// RIGID BODY
// A rigid body is the process of using physics 
// to control the movement and position of objects
struct Rigid_Body {
	V2 pos_ws;
	V2 vel;
	int speed;
	// acceleration
	// V2 pos_ls
	// Colliders
};

struct Player {
	Image* image;

	Rigid_Body rb;

	int w, h;

	// int health;
	// int damage;
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

Rigid_Body create_rigid_body(V2 pos_ws, int speed);
Player create_player(Image* image, V2 spawn_pos_ws, int player_speed);

Arrow create_arrow(Image* image, V2 pos, V2 vel, int width, int height, int speed);
void update_arrow(Arrow& arrow, float delta_time);
void draw_arrow(int camera_pos_x, int camera_pos_y, Arrow& arrow);

// void spawn_entity(V2_F pos);
// void update_entity();
// void draw_entity();
