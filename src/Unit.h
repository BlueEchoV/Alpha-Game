#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "Player.h"

#include <vector>

enum Unit_Type {
	UT_Zombie,
	UT_Total_Unit_Types
};

struct Unit_Data {
	std::string unit_name;
	int w;
	int h;
	int health;
	int damage;
	float attacks_per_sec;
	int speed;
};

struct Player;

struct Unit {
	Faction faction;
	std::string unit_name;
	Animation_Tracker at;
	Health_Bar health_bar;
	Rigid_Body rb;
	int w, h;

	Player* target;

	int damage;

	bool dead = false;
	bool destroyed = false;

	bool can_attack = true;
	Cooldown attack_cd;

	Handle handle;
};

void spawn_unit(Faction faction, std::string unit_name, Animation_State as, Storage<Unit>& storage, 
	std::vector<Handle>& handles, Player* target, V2 spawn_pos);
void update_unit(Unit& unit, float dt);
void draw_unit(Unit& unit, V2 camera_pos);

Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle);

void load_unit_data_csv(CSV_Data* data);

