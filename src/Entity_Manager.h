#pragma once
#include "Utility.h"

#include <vector>

enum Storage_Type : uint8_t {
	// Init it to 1 so that null inits aren't value
	ST_Unit = 1,
	ST_Projectile,
	ST_Building,
	ST_Draw_Order
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
    Storage_Type storage_type;
    std::vector<Generations> generations;
    std::vector<T> storage;

    // Constructor to initialize with an optional initial capacity
	Storage(Storage_Type type, size_t initial_capacity = MAX_STORAGE_SIZE) : storage_type(type) {
			generations.reserve(initial_capacity);
			storage.reserve(initial_capacity);
}};

template <typename T>
Handle create_handle(Storage<T>& storage) {
    Handle result = {};
    size_t length = storage.storage.size();
    for (size_t i = 0; i < length; ++i) {
        if (!storage.generations[i].slot_is_taken) {
            storage.generations[i].slot_is_taken = true;
            result.index = static_cast<uint32_t>(i);
            result.generation = storage.generations[i].current_slot_generation;
            result.storage_type = storage.storage_type;
            return result;
        }
    }
    // No free slot found: resize by doubling capacity
    size_t new_size = (length == 0) ? 1 : length * 2;
    storage.generations.resize(new_size, Generations{false, 1}); // Initialize new slots
    storage.storage.resize(new_size);
    // Use the first new slot
    size_t new_index = length;
    storage.generations[new_index].slot_is_taken = true;
    result.index = static_cast<uint32_t>(new_index);
    result.generation = storage.generations[new_index].current_slot_generation;
    result.storage_type = storage.storage_type;
    return result;
}

#undef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

template <typename T>
void delete_handle(Storage<T>& storage, Handle handle) {
    uint32_t index = handle.index;
    uint32_t generation = handle.generation;
    if (generation == 0) {
        return; // Reject uninitialized handles
    }
    if (index < storage.storage.size() &&
        generation == storage.generations[index].current_slot_generation &&
        storage.storage_type == handle.storage_type) {
        uint32_t new_generation = storage.generations[index].current_slot_generation + 1;
        storage.generations[index].current_slot_generation = (new_generation < UINT32_MAX) ? new_generation : UINT32_MAX;
        storage.generations[index].slot_is_taken = false;
    }
}

template <typename T>
T* get_entity_pointer_from_handle(Storage<T>& storage, Handle handle) {
    uint32_t index = handle.index;
    uint32_t generation = handle.generation;
    if (index < storage.storage.size() &&
        storage.generations[index].current_slot_generation == generation &&
        storage.generations[index].slot_is_taken &&
        storage.storage_type == handle.storage_type) {
        return &storage.storage[index];
    }
    return nullptr;
}