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

Player create_player(Image* image, int player_speed);

// void spawn_entity(MP_Renderer* renderer, V2_F pos);
// void update_entity();
// void draw_entity();
