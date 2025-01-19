#pragma once
#include "Utility.h"
#include "renderer.h"
#include "Image.h"

struct MP_Renderer;

struct Player {
	Image* image;

	// World Space
	V2 position_ws;
	// Camera Space
	V2 position_cs;

	int w, h;

	// int health;
	// int damage;
	int speed;
};

struct Arrow {
	V2 pos;
	V2 vel;
	int speed;

	Image* image;
};

Player create_player(Image* image, int player_speed);

Arrow create_arrow(Image* image, V2 pos, int speed);
void update_arrow(Arrow& arrow);
void draw_arrow(MP_Renderer* renderer, Arrow& arrow);

// void spawn_entity(MP_Renderer* renderer, V2_F pos);
// void update_entity();
// void draw_entity();
