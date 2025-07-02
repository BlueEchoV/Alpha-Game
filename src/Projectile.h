#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Rigidbody.h"
#include "Renderer.h"

enum Projectile_Type {
	PT_Arrow
};

struct Projectile_Data {
	std::string projectile_name;

	int w, h;
	int speed;
};

struct Projectile {
	Rigid_Body rb;

	int w, h;

	Animation_Tracker at;

	bool destroyed = false;
	int damage;

	Handle handle;
};

// Damage based off weapon
void spawn_projectile(std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage,
	std::string_view projectile_name, int damage, V2 origin_ws, V2 target_ws);
void update_projectile(Projectile& projectile, float delta_time);
void draw_projectile(int camera_pos_x, int camera_pos_y, Projectile& projectile);

void load_projectile_data_csv(CSV_Data* data);
