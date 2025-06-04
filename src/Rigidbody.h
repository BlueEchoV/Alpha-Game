#pragma once
#include "Sprite_Sheet.h"

// Colliders
struct Collider {
	float radius;
	// This is offset based off the position of the rigid body
	V2 pos_ls;
};

// RIGID BODY
// A rigid body is the process of using physics 
// to control the movement and position of objects
struct Rigid_Body {
	V2 pos_ws;
	V2 vel;

	float angle;

	int base_speed;
	int current_speed;

	int num_colliders;
	Collider colliders[Globals::MAX_COLLIDERS];
	// acceleration
	// V2 pos_ls
};

V2 convert_cs_to_ws(V2 entity_pos, V2 camera_pos);
V2 convert_cs_to_ws(int entity_x, int entity_y, int camera_x, int camera_y);
V2 convert_ws_to_cs(V2 entity_pos, V2 camera_pos);
V2 convert_ws_to_cs(int entity_x, int entity_y, int camera_x, int camera_y);

void draw_circle_ws(Color_Type c, V2 center_pos_ws, int radius, float total_lines);
void draw_circle_cs(Color_Type c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines);

void add_collider(Rigid_Body* rb, V2 pos_ls, float radius);
bool check_rb_collision(Rigid_Body* rb_1, Rigid_Body* rb_2);
void draw_colliders(Rigid_Body* rb, V2 camera_pos);
Rigid_Body create_rigid_body(V2 pos_ws, int speed);
