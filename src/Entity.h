#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Renderer.h"
#include "Rigidbody.h"

#include <vector>

struct MP_Renderer;
struct Game_Data;

enum Unit_Type {
	UT_Zombie,
	UT_Total_Unit_Types
};

enum Storage_Type : uint8_t {
	// Init it to 1 so that null inits aren't value
	ST_Unit = 1,
	ST_Projectile,
	ST_Building
};

const int MAX_STORAGE_SIZE = 1000;

// This goes on the entities
struct Handle {
	Storage_Type storage_type;
	uint32_t index;
	// Used to compare against generations. Does not change. 
	// This is set equal to the generation's counter on creation
	uint32_t generation;
};

// Each array slot gets its own generation counter, 
// which is bumped when a handle is released
struct Generations {
	bool slot_is_taken = false;
	// Used to compare against handles
	uint32_t current_slot_generation = 1;
};

template <typename T>
struct Storage {
	// Will need a variable (index_one_past_last) for serialization
	Storage_Type storage_type;
	Generations generations[MAX_STORAGE_SIZE];
	T storage[MAX_STORAGE_SIZE];
};

// Creates a entity for a specific storage
template <typename T>
Handle create_handle(Storage<T>& storage) {
	Handle result = {};
	int length = ARRAYSIZE(storage.storage);
	for(int i = 0; i < length; i++) { 
		if (storage.generations[i].slot_is_taken == false) {
			storage.generations[i].slot_is_taken = true;
			result.index = i;
			result.generation = storage.generations[i].current_slot_generation;
			break;
		} 
	} 

	result.storage_type = storage.storage_type;
	return result;
}

#undef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

template <typename T>
void delete_handle(Storage<T>& storage, Handle handle) {
	uint32_t index = handle.index;
	uint32_t generation = handle.generation;
	if (index < ARRAYSIZE(storage.storage) && 
		generation == storage.generations[index].current_slot_generation &&
		storage.storage_type == handle.storage_type){
		storage.generations->current_slot_generation++;
		storage.generations->slot_is_taken = false;
	}
}

// NOTE: I need to make sure and check if it returns null. If it does,
// DON'T USE IT.
template <typename T>
T* get_entity_pointer_from_handle(Storage<T>& storage, Handle handle) {
	uint32_t index = handle.index;
	uint32_t generations = handle.generation;
	// Check if the handle is valid
	if (storage.generations[index].current_slot_generation == generations &&
		storage.generations[index].slot_is_taken == true &&
		storage.storage_type == handle.storage_type) {
		return &storage.storage[index];
	}
	log("Entity does not exist from handle");
	// assert(false);
	return nullptr;
}

struct Unit_Data {
	std::string unit_name;
	int w;
	int h;
	int health;
	int damage;
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

	int damage;
	Player* target;

	bool dead = false;
	bool destroyed = false;

	Handle handle;
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

enum Projectile_Type {
	PT_Arrow
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

	void fire_weapon(Game_Data& game_data, Faction faction);
	void update_weapon(float delta_time);
	// virtual void reload(); // Reloading animation? // This could be tedious
	// Should this be here?
	// IN PROGRESS
	void draw_weapon(Animation_Tracker* at, V2 pos);
};

void equip_weapon(Weapon*& weapon, std::string weapon_name);
void unequip_weapon();
void delete_weapon(Weapon*& weapon);

void load_unit_data_csv(CSV_Data* data);

struct Player_Data {
	std::string character_name;
	int hp;
	int base_speed;
	int damage;
	int w, h;
};

struct Player {
	Faction faction = F_Player;
	std::string character_name;
	Animation_Tracker at;

	Rigid_Body rb;
	Health_Bar health_bar;

	int w, h;

	// Could this all be put into a 'weapon_kit' of some sort?
	Weapon* weapon;

	int damage;
};

// Make sure and release weapon memory
void delete_player();

struct Camera {
	V2 pos_ws;
	int w, h;
};

enum Horde_Type {
	HT_Not_Specified,
	HT_Basic
};

struct Horde_Data {
	int level;
	int total_to_spawn;
	// In seconds
	float max_spawning_cd;
	// Zombie Types
};

struct Horde {
	Faction faction;
	int level;
	MP_Rect spawn_region_ws;
	bool begin_spawning;
	int total_to_spawn;
	int total_spawned;

	Cooldown spawning_cd;
}; 

struct Game_Data {
	Camera camera;

	Player player;

	Storage<Projectile> projectile_storage = { .storage_type = ST_Projectile };
	Storage<Unit>		unit_storage = { .storage_type = ST_Unit };

	std::vector<Handle> enemy_unit_handles;
	std::vector<Handle> projectile_handles;

	Horde current_horde;

	Font_Type selected_font;
};

Player create_player(std::string character_name, V2 spawn_pos_ws);
void draw_player(Player& p, V2 camera_ws_pos);

void spawn_unit(Faction faction, std::string unit_name, Animation_State as, Storage<Unit>& storage, 
	std::vector<Handle>& handles, Player* target, V2 spawn_pos);
void update_unit(Unit& unit, float dt);
void draw_unit(Unit& unit, V2 camera_pos);
Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle);
void delete_destroyed_entities_from_handles(Game_Data& game_data);
Horde create_horde(Faction faction, Horde_Type ht, MP_Rect rect);
void spawn_and_update_horde(Game_Data& game_data, float delta_time);
void draw_horde_spawn_region(Color_Type c, Horde& horde, V2 camera_pos);

void spawn_projectile(Game_Data& game_data, std::string projectile_name, int damage, V2 origin_ws, V2 target_ws);
void update_projectile(Projectile& projectile, float delta_time);
void draw_projectile(int camera_pos_x, int camera_pos_y, Projectile& projectile);

void load_weapon_data_csv(CSV_Data* data);
void load_projectile_data_csv(CSV_Data* data);
// void spawn_entity(V2_F pos);
// void update_entity();
// void draw_entity();
