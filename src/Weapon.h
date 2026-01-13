#pragma 
#include "Utility.h"
#include "Game_Utility.h"
#include "Sprite_System.h"
#include "Projectile.h"
#include "Menu.h"

struct Damage_Number {
	int damage;
	V2 pos_ws;
	V2 velocity;
	float current_lifetime;
	float total_lifetime;
	Animation_Tracker background_at;
	// Is a crit
	// Alpha value for fading
};

void create_and_add_damage_number(std::vector<Damage_Number>& damage_numbers, V2 pos_ws, V2 vel_normalized, float speed,
	int damage, float life_time, std::string& background_image_name);
void update_damage_numbers(std::vector<Damage_Number>& damage_numbers, const float dt);
void draw_damage_numbers(Font& font, std::vector<Damage_Number>& damage_numbers, V2 camera_pos_ws);

struct Weapon_Data {
	std::string weapon_name;

	// The actual width and height of the weapon on screen
	int damage;
	int attacks_per_sec;
	int max_ammo;
	int reload_per_sec;

	std::string projectile_name;
	int projectile_speed;
	int projectile_w;
	int projectile_h;
	float projectile_lifespan;
};

enum Weapon_Type {
	WT_Bow,
	WT_Total
};

struct Weapon {
	std::string weapon_name;

	int weapon_w;
	int weapon_h;

	bool uses_ammo;
	int max_ammo;
	float reload_interval;
	float reload_intervals_to_full;
	int ammo;
	int base_damage;
	int damage;
	// bool can_stick

	int attacks_per_sec;
	int reload_per_sec;
	bool can_fire = true;
	float fire_cooldown;
	float reload_progress;
	float reload_cooldown;

	// PROJECTILES
	int projectile_w;
	int projectile_h;
	int projectile_speed;
	std::string projectile_name;
	float projectile_lifespan;

	void fire_weapon(Camera camera, V2 spawn_pos_ws, V2 target_pos_ws, Faction faction,
		Storage<Projectile>& projectile_storage, std::vector<Handle>& projectile_handles,
		Storage<Draw_Order>& draw_order_storage, std::vector<Handle>& draw_order_handles);
	void update_weapon(float delta_time);
	void update_max_ammo_size(int new_size);
	void update_reload_interval(int new_reload_per_sec);
	void draw_ui(V2 hb_pos_cs);
	// virtual void reload(); // Reloading animation? // This could be tedious
};

struct Sword : Weapon {

};

struct Crossbow : Weapon {

};

void equip_weapon(Weapon*& weapon, std::string weapon_name);
void delete_weapon(Weapon*& weapon);

// Draw weapon UI (Ammo and whatnot)

void load_weapon_data_csv(CSV_Data* data);

