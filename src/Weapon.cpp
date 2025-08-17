#include "Weapon.h"

std::unordered_map<std::string, Weapon_Data> weapon_data_map;

Weapon_Data bad_weapon_data = {
	// weapon_name	weapon_w	weapon_h		damage		attack_speed	proj_name proj_speed	proj_w	proj_h
	 "bow",			50,			50,				10,			4,				"arrow",  50,			100,	100	
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
	weapon->weapon_w = current_weapon_data.weapon_w;
	weapon->weapon_h = current_weapon_data.weapon_h;
	weapon->base_damage = current_weapon_data.damage;
	weapon->damage = current_weapon_data.damage;
	weapon->attacks_per_second = current_weapon_data.attacks_per_second;

	weapon->projectile_name = current_weapon_data.projectile_name;
	weapon->projectile_w = current_weapon_data.projectile_w;
	weapon->projectile_h = current_weapon_data.projectile_h;
	weapon->projectile_speed = current_weapon_data.projectile_speed;
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
			V2 mouse_cs_pos = get_viewport_mouse_position(Globals::renderer->open_gl.window_handle);
			V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, camera.pos_ws);
			spawn_projectile(projectile_handles, projectile_storage, this->projectile_name, this->damage, 
				this->projectile_speed, this->projectile_w, this->projectile_h, spawn_pos_ws, mouse_ws_pos);
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

Type_Descriptor weapon_data_type_descriptors[] = {
	FIELD(Weapon_Data, VT_String, weapon_name),
	FIELD(Weapon_Data, VT_Int, weapon_w),
	FIELD(Weapon_Data, VT_Int, weapon_h),
	FIELD(Weapon_Data, VT_Int, damage),
	FIELD(Weapon_Data, VT_Int, attacks_per_second),
	FIELD(Weapon_Data, VT_String, projectile_name),
	FIELD(Weapon_Data, VT_Int, projectile_w),
	FIELD(Weapon_Data, VT_Int, projectile_h),
	FIELD(Weapon_Data, VT_Int, projectile_speed)
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

