#pragma once
#include "Utility.h"

#include <vector>

enum Storage_Type : uint8_t {
	// Init it to 1 so that null inits aren't value
	ST_Unit = 1,
	ST_Projectile,
	ST_Building
};

const int MAX_STORAGE_SIZE = 100;

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
        storage.storage_type == handle.storage_type) {
        storage.generations[index].current_slot_generation++;
        storage.generations[index].slot_is_taken = false;
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
	// assert(false);
	return nullptr;
}

