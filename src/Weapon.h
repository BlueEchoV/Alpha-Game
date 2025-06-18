#pragma 
#include "Utility.h"
#include "Game_Utility.h"
#include "Sprite_Sheet.h"
#include "Projectile.h"

struct Weapon_Data {
	std::string weapon_name;
	std::string projectile_name;

	// The actual width and height of the weapon on screen
	int w;
	int h;
	int damage;
	int attacks_per_second;
};

enum Weapon_Type {
	WT_Bow,
	WT_Total
};

struct Weapon {
	std::string weapon_name;
	std::string projectile_name;

	// Sprite sheet
	Image* image;
	int w, h;
	int attacks_per_second;
	int damage;
	// bool can_stick

	bool can_fire = true;
	float fire_cooldown;

	void fire_weapon(std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage, 
		Camera camera, V2 spawn_pos_ws, Faction faction);
	void update_weapon(float delta_time);
	// virtual void reload(); // Reloading animation? // This could be tedious
	// Should this be here?
	// IN PROGRESS
	void draw_weapon(Animation_Tracker* at, V2 pos);
};

void equip_weapon(Weapon*& weapon, std::string weapon_name);
void unequip_weapon();
void delete_weapon(Weapon*& weapon);


void load_weapon_data_csv(CSV_Data* data);

