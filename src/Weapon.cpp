#include "Weapon.h"

std::unordered_map<std::string, Weapon_Data> weapon_data_map;

Weapon_Data bad_weapon_data = {
	// weapon_name	proj_name	w		h		damage		fire_rate
	 "bow",			"arrow",	50,		50,		10,			4			
};

Weapon_Data get_weapon_data(std::string weapon_name) {
	Weapon_Data result = {};
	auto it = weapon_data_map.find(weapon_name);
	if (it == weapon_data_map.end()) {
		return bad_weapon_data;
	}

	return it->second;
};

// Pass the pointer as a reference so we can modify the actual address it pointing at
void equip_weapon(Weapon*& weapon, std::string weapon_name) {
	if (weapon == NULL) {
		// New re-assigns the pointer to point to a new location in memory
		weapon = new Weapon();
	}
	*weapon = {};

	Weapon_Data current_weapon_data = get_weapon_data(weapon_name);
	weapon->weapon_name = current_weapon_data.weapon_name;
	weapon->projectile_name = current_weapon_data.projectile_name;
	weapon->w = current_weapon_data.w;
	weapon->h = current_weapon_data.h;
	weapon->damage = current_weapon_data.damage;
	weapon->attacks_per_second = current_weapon_data.attacks_per_second;
}

void unequip_weapon();

void delete_weapon(Weapon*& weapon) {
	delete weapon;
	// No dangling pointers
	weapon = nullptr;
}

// Fires at the mouse
void Weapon::fire_weapon(std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage, 
	Camera camera, V2 spawn_pos_ws, Faction faction) {
	switch (faction) {
	case F_Player: {
		if (this->can_fire) {
			// Change this to fire weapon
			V2 mouse_cs_pos = get_mouse_position(Globals::renderer->open_gl.window_handle);
			V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, camera.pos_ws);
			spawn_projectile(projectile_handles, projectile_storage, this->projectile_name, this->damage, spawn_pos_ws, mouse_ws_pos);
			Globals::debug_total_arrows++;
			this->can_fire = false;
		}
		break;
	}
	case F_Allies: {
		break;
	}
	case F_Enemies: {
		break;
	}
	default: {
		log("Entity_Type not specified");
		break;
	}
	}

}

void Weapon::update_weapon(float delta_time) {
	if (this != nullptr) {
		if (this->fire_cooldown <= 0) {
			this->can_fire = true;
			this->fire_cooldown = 1.0f / (float)this->attacks_per_second;
		}
		if (this->can_fire == false) {
			this->fire_cooldown -= delta_time;
		}
	}
}

// This will have to be an offset based off where the player is looking
void Weapon::draw_weapon(Animation_Tracker* at, V2 pos) {
	MP_Rect dst = {(int)pos.x, (int)pos.y, this->w, this->h};

	draw_animation_tracker(at, dst, 0);
}

Type_Descriptor weapon_data_type_descriptors[] = {
	FIELD(Weapon_Data, VT_String, weapon_name),
	FIELD(Weapon_Data, VT_String, projectile_name),
	FIELD(Weapon_Data, VT_Int, w),
	FIELD(Weapon_Data, VT_Int, h),
	FIELD(Weapon_Data, VT_Int, damage),
	FIELD(Weapon_Data, VT_Int, attacks_per_second)
};

void load_weapon_data_csv(CSV_Data* data) {
	std::vector<Weapon_Data> weapon_data;

	weapon_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_weapon_data_type_descriptors(weapon_data_type_descriptors);

	load_csv_data_file(data, (char*)weapon_data.data(), safe_weapon_data_type_descriptors, sizeof(Weapon_Data));

	for (Weapon_Data& weapon_data_iterator : weapon_data) {
		weapon_data_map[weapon_data_iterator.weapon_name] = weapon_data_iterator;
	}
}

