#pragma once
#include "Utility.h"
#include "renderer.h"
#include "Image.h"
#include <vector>

struct MP_Renderer;

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

// Colliders
struct Collider {
	float radius;
	// This is offset based off the position of the rigid body
	V2 pos_ls;
};

// RIGID BODY
// A rigid body is the process of using physics 
// to control the movement and position of objects
struct Rigid_Body {
	V2 pos_ws;
	V2 vel;

	float angle;

	int speed;

	int num_colliders;
	Collider colliders[Globals::MAX_COLLIDERS];
	// acceleration
	// V2 pos_ls
};

struct Unit_Data {
	std::string unit_name;
	std::string image_name;
	int w;
	int h;
	int health;
	int damage;
	int speed;
};

struct Player;

struct Unit {
	Image* image;
	Rigid_Body rb;
	int w, h;

	int health;
	int damage;
	Player* target;

	bool destroyed = false;

	Handle handle;
};

struct Projectile_Data {
	std::string projectile_type;
	Image_Type it;

	int w, h;
	int speed;
};

struct Projectile {
	Rigid_Body rb;

	int w, h;
	float angle;

	Image* image;

	bool destroyed = false;

	Handle handle;
};

struct Weapon_Data {
	std::string weapon_type;
	std::string image_name;
	std::string projectile_type;
	// The actual width and height of the weapon on screen
	int w;
	int h;
	int damage;
	int fire_rate;
};

enum Weapon_Type {
	WT_Bow,
	WT_Total
};

enum Projectile_Type {
	PT_Arrow
};

struct Weapon {
	std::string type;

	// Sprite sheet
	Image* image;
	int w, h;
	int fire_rate;
	int damage;
	// bool can_stick

	bool can_fire = true;
	float fire_cooldown;
};

void load_weapon_data_csv(CSV_Data* data);

struct Game_Data;

struct Player {
	Image* image;

	Rigid_Body rb;

	int w, h;

	// Could this all be put into a 'weapon_kit' of some sort?
	Weapon* weapon;
	void equip_weapon(Weapon_Type wt);
	void fire_weapon(Game_Data& game_data);
	void update_weapon(float delta_time);
	// virtual void reload(); // Reloading animation? // This could be tedious
	// Should this be here?
	void draw_weapon();

	// int health;
	// int damage;
};

// Make sure and release weapon memory
void delete_player();

struct Camera {
	V2 pos_ws;
	int w, h;
};

struct Game_Data {
	Camera camera;

	Player player;

	Storage<Projectile> projectile_storage = { .storage_type = ST_Projectile };
	Storage<Unit>		unit_storage = { .storage_type = ST_Unit };

	std::vector<Handle> enemy_unit_handles;
	std::vector<Handle> projectile_handles;

	Font_Type selected_font;
};

// Zombie* get_zombie_from_handle(Handle handle);

V2 convert_cs_to_ws(V2 entity_pos, V2 camera_pos);
V2 convert_cs_to_ws(int entity_x, int entity_y, int camera_x, int camera_y);
V2 convert_ws_to_cs(V2 entity_pos, V2 camera_pos);
V2 convert_ws_to_cs(int entity_x, int entity_y, int camera_x, int camera_y);

void draw_circle_ws(Color_Type c, V2 center_pos_ws, int radius, float total_lines);
void draw_circle_cs(Color_Type c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines);

void add_collider(Rigid_Body* rb, V2 pos_ls, float radius);
void draw_colliders(Rigid_Body* rb, V2 camera_pos);
Rigid_Body create_rigid_body(V2 pos_ws, int speed);

Player create_player(Image* image, V2 spawn_pos_ws, int player_speed);
void draw_player(Player& p, V2 camera_ws_pos);

void spawn_unit(Unit_Type unit_type, Storage<Unit>& storage, std::vector<Handle>& handles,
	Player* target, V2 spawn_pos);
void update_unit(Unit& unit, float dt);
void draw_unit(Unit& unit, V2 camera_pos);
Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle);
void delete_destroyed_entities_from_handles(Game_Data& game_data);

void spawn_projectile(Game_Data& game_data, Projectile_Type pt, V2 origin, V2 target);
void update_projectile(Projectile& projectile, float delta_time);
void draw_projectile(int camera_pos_x, int camera_pos_y, Projectile& projectile);

// void spawn_entity(V2_F pos);
// void update_entity();
// void draw_entity();
