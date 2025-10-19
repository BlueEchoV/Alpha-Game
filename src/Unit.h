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

enum Unit_State {
	US_Idle,
	US_Moving,
	US_Attacking,
	US_Dying
};

struct Player;

struct Unit {
	Faction faction;
	std::string unit_name;
	Unit_State us;

	Animation_Tracker at;
	Health_Bar health_bar;
	Rigid_Body rb;
	int w, h;

	Player* target;

	int damage;

	bool dead = false;
	Handle draw_order_handle;
	bool destroyed = false;

	bool can_attack = true;
	Cooldown attack_cd;
    bool damage_applied = false; // To guarrantee multiple attacks don't hit in the same cycle
    int attack_hit_frame = 8;  // Per-unit or load from data

	Handle handle;
};

void spawn_unit(Faction faction, std::string_view unit_name, Animation_State starting_as,
	Animation_Play_Speed starting_aps, Animation_Mode starting_am, Player* target, V2 spawn_pos,
	Storage<Unit>& storage, std::vector<Handle>& handles,
	Storage<Draw_Order>& draw_order_storage, std::vector<Handle>& draw_order_handles);
void kill_unit(Unit& unit, int& active_enemy_units_counter);
void update_unit(Player& p, Unit& unit, float dt);
void render_unit(Unit& unit, V2 camera_pos);
void render_unit_outlined(Unit& unit, V2 camera_pos, Color_Type outline_color, float outline_thickness);

Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle);

void load_unit_data_csv(CSV_Data* data);

