#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Weapon.h"
#include "Tile_Map.h"

struct Ability;

struct Player_Data {
	std::string character_name;
	int hp;
	int base_speed;
	int damage;
	int w, h;
};

struct Player {
	bool dead;

	Faction faction = F_Player;
	std::string character_name;

	Animation_Tracker torso;
	Animation_Tracker legs;

	Rigid_Body rb;
	Health_Bar health_bar;

	int w, h;

	// Could this all be put into a 'weapon_kit' of some sort?
	Weapon* weapon;

	Ability* passive;
	Ability* basic;
	Ability* ultimate;

	int damage;
};

Player create_player(std::string_view character_name, V2 spawn_pos_ws);
void draw_player(Player& p, V2 camera_ws_pos);

// Torso - All 16 degrees
// Legs - NW N NE  
