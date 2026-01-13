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
};

struct Projectile {
	Rigid_Body rb;

	int w, h;

	Animation_Tracker at;

	Handle draw_order_handle;
	bool destroyed = false;
	int damage;

	float lifespan;

	Handle handle;
};

// Damage based off weapon
void spawn_projectile(std::string_view projectile_name, int damage, int speed, int w, int h, float projectile_lifespan, V2 origin_ws, V2 target_ws, 
	std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage,
	Storage<Draw_Order>& draw_order_storage, std::vector<Handle>& draw_order_handles);
void update_projectile(Projectile& projectile, float delta_time);
void render_projectile(Projectile& projectile, V2 camera_pos_ws);

// void load_projectile_data_csv(CSV_Data* data);
