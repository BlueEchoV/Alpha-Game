#pragma 
#include "Utility.h"
#include "Game_Utility.h"
#include "Sprite_System.h"
#include "Projectile.h"

struct Weapon_Data {
	std::string weapon_name;

	// The actual width and height of the weapon on screen
	int weapon_w;
	int weapon_h;
	int damage;
	int attacks_per_second;

	std::string projectile_name;
	int projectile_speed;
	int projectile_w;
	int projectile_h;
};

enum Weapon_Type {
	WT_Bow,
	WT_Total
};

struct Weapon {
	std::string weapon_name;

	int weapon_w;
	int weapon_h;

	int attacks_per_second;
	int base_damage;
	int damage;
	// bool can_stick

	bool can_fire = true;
	float fire_cooldown;

	// PROJECTILES
	int projectile_w;
	int projectile_h;
	int projectile_speed;
	std::string projectile_name;

	void fire_weapon(std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage, 
		Camera camera, V2 spawn_pos_ws, Faction faction);
	void update_weapon(float delta_time);
	// virtual void reload(); // Reloading animation? // This could be tedious
};

void equip_weapon(Weapon*& weapon, std::string weapon_name);
void unequip_weapon();
void delete_weapon(Weapon*& weapon);

// Draw weapon UI (Ammo and whatnot)

void load_weapon_data_csv(CSV_Data* data);

