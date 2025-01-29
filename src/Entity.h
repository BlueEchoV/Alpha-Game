#pragma once
#include "Utility.h"
#include "renderer.h"
#include "Image.h"

struct MP_Renderer;

enum Storage_Type {
	ST_Entity
};

const int max_storage_size = 100;

// This goes on the entities
struct Handle {
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
	int current_slot_generation = 1;
};

template <typename T>
struct Storage {
	// Will need a variable (index_one_past_last) for serialization
	Storage_Type storage_type;
	Generations generations[max_storage_size];
	T storage[max_storage_size];
};

// Creates a entity for a specific storage
template <typename T>
Handle create_handle(Storage<T> storage) {
	Handle result = {};
	int length = ARRAYSIZE(storage);
	for(int i = 0; i < length; i++) { 
		if (storage.generations[i].slot_is_taken == false) {
			storage.generations[i].slot_is_taken = true;
			result.index = i;
			result.generation = storage.generations[i].current_slot_generation;
		} 
	} 

	// Returning a null handle if there is not room

	return result;
}

template <typename T>
void delete_handle(Storage<T>& storage, Handle handle) {
	int index = handle.index;
	if (index < ARRAYSIZE(storage) && 
		generation == storage.generations[index].current_slot_generation){
		storage.generations->current_slot_generation++;
		storage.generations->slot_is_taken = false;
	}
}

// Colliders
struct Collider {
	float rad;
	// This is offset based off the position of the rigid body
	V2 pos_ls;
};

// RIGID BODY
// A rigid body is the process of using physics 
// to control the movement and position of objects
struct Rigid_Body {
	V2 pos_ws;
	V2 vel;
	int speed;
	// acceleration
	// V2 pos_ls
	// Colliders
};

struct Player {
	Image* image;

	Rigid_Body rb;

	int w, h;

	// int health;
	// int damage;
};

struct Zombie {
	Image* image;
	Rigid_Body rb;
	int w, h;

	int health;
	int damage;
	Player* target;

	Handle handle;
};

struct Entity {
	Image* image;
	Rigid_Body rb;
	int w, h;

	int health;
	int damage;

	// This will have the be variable in the future
	Player* target;
};

struct Arrow {
	V2 pos_ws;
	V2 vel;
	int speed;
	int w, h;
	float angle;

	Image* image;
};

V2 convert_cs_to_ws(V2 entity_pos, V2 camera_pos);
V2 convert_cs_to_ws(int entity_x, int entity_y, int camera_x, int camera_y);
V2 convert_ws_to_cs(V2 entity_pos, V2 camera_pos);
V2 convert_ws_to_cs(int entity_x, int entity_y, int camera_x, int camera_y);

V2 calculate_origin_to_target_velocity(V2 target, V2 origin);

Rigid_Body create_rigid_body(V2 pos_ws, int speed);
Player create_player(Image* image, V2 spawn_pos_ws, int player_speed);
Zombie spawn_zombie(Image* image, Player* target, V2 spawn_pos,
	int width, int height, int speed, int health, int damage);
void update_zombie(Zombie& zombie, float dt);
void draw_zombie(Zombie& zombie, V2 camera_pos);

Arrow create_arrow(Image* image, V2 pos, V2 vel, int width, int height, int speed);
void update_arrow(Arrow& arrow, float delta_time);
void draw_arrow(int camera_pos_x, int camera_pos_y, Arrow& arrow);

// void spawn_entity(V2_F pos);
// void update_entity();
// void draw_entity();
