#include "Weapon.h"

std::unordered_map<std::string, Weapon_Data> weapon_data_map;

Weapon_Data bad_weapon_data = {
	// weapon_name	weapon_w	weapon_h		damage		attacks_per_second ammo		reload_per_sec proj_name proj_speed	proj_w	proj_h
	 "crossbow",	50,			50,				10,			4,				   0,		1,			   "blot",  50,			100,	100	
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
	else {
		delete_weapon(weapon);
	}
	*weapon = {};

	Weapon_Data current_weapon_data = get_weapon_data(weapon_name);
	weapon->weapon_name = current_weapon_data.weapon_name;
	weapon->weapon_w = current_weapon_data.weapon_w;
	weapon->weapon_h = current_weapon_data.weapon_h;
	weapon->base_damage = current_weapon_data.damage;
	weapon->damage = current_weapon_data.damage;
	weapon->attacks_per_sec = current_weapon_data.attacks_per_sec;
	weapon->reload_per_sec = current_weapon_data.reload_per_sec;
	weapon->max_ammo = current_weapon_data.max_ammo;
	weapon->ammo = 0;

	weapon->projectile_name = current_weapon_data.projectile_name;
	weapon->projectile_w = current_weapon_data.projectile_w;
	weapon->projectile_h = current_weapon_data.projectile_h;
	weapon->projectile_speed = current_weapon_data.projectile_speed;
}

void delete_weapon(Weapon*& weapon) {
	delete weapon;
	// No dangling pointers
	weapon = nullptr;
}

// Fires at the mouse
void Weapon::fire_weapon(std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage, 
	Camera camera, V2 spawn_pos_ws, Faction faction) {
	// TODO: This currently is obsolete. I might add it back later. 
	switch (faction) {
	case F_Player: {
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

	if (this->can_fire && this->ammo > 0) {
		std::string current_ammo = std::to_string(this->ammo);
		log(current_ammo.c_str());
		this->ammo--;
		// printf("Subtracting ammo. Current: %i", this->ammo);
		// Change this to fire weapon
		V2 mouse_cs_pos = get_viewport_mouse_position(Globals::renderer->open_gl.window_handle);
		V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, camera.pos_ws);
		spawn_projectile(projectile_handles, projectile_storage, this->projectile_name, this->damage, 
			this->projectile_speed, this->projectile_w, this->projectile_h, spawn_pos_ws, mouse_ws_pos);
		Globals::debug_total_arrows++;
		this->can_fire = false;
		this->fire_cooldown = 1.0f / this->attacks_per_sec;
		if (this->reload_progress >= (1.0f / this->reload_per_sec)) { 
			this->reload_progress -= 1.0f / this->reload_per_sec;
		}
	}
}

void Weapon::update_weapon(float delta_time) {
	if (this->fire_cooldown <= 0.0f) {
		if (this->ammo < this->max_ammo || this->ammo > 0 && this->can_fire == false) {
			this->can_fire = true;
		} 	
	} else {
		this->fire_cooldown -= delta_time;
	}

	// Uses ammo
	if (this->reload_per_sec > 0.0f) {
		float one_round_reload_time = (1.0f / this->reload_per_sec);
		float total_clip_reload_time = (float)this->max_ammo * one_round_reload_time;

		this->reload_progress += delta_time;
		if (this->reload_progress > total_clip_reload_time) {
			this->reload_progress = total_clip_reload_time;
		}
		clamp(this->reload_progress, 0.0f, total_clip_reload_time);

		if (this->reload_cooldown <= 0.0f) {
			// Make sure it's within the range
			if (this->reload_progress >= one_round_reload_time && this->reload_progress <= total_clip_reload_time) {
				ammo++;
			}

			if (this->ammo <= this->max_ammo) {
				this->reload_cooldown = 1.0f / this->reload_per_sec;
			}
			else {
				this->reload_cooldown = 0.0f;
			}
		}
		else {
			this->reload_cooldown -= delta_time;
		}
	}
}

void Weapon::update_ammo_size(int new_size) {
	this->max_ammo = new_size;
	this->ammo = 0;
}

// Draw it relative to the health bar
void Weapon::draw_ui(V2 hb_pos_cs) {
	// Font* font = get_font(FT_Basic);

	// Draw the ammo bar
	if (this->max_ammo > 0) {
		int total_health_bar_w = Globals::DEFAULT_HEALTH_BAR_WIDTH + Globals::DEFAULT_RESOURCE_BAR_OUTLINE * 2;
		REF(total_health_bar_w);
		int inner_health_bar_w = Globals::DEFAULT_HEALTH_BAR_WIDTH;
		// This includes the outline around the health bar
		int total_health_bar_h = Globals::DEFAULT_HEALTH_BAR_HEIGHT + Globals::DEFAULT_RESOURCE_BAR_OUTLINE * 2;

		int ammo_rect_h = Globals::DEFAULT_RESOURCE_BAR_H;
		int ammo_rect_w = inner_health_bar_w / this->max_ammo;

		int starting_x = (int)hb_pos_cs.x - (inner_health_bar_w / 2);
		int starting_y = (int)hb_pos_cs.y - (total_health_bar_h / 2) - ammo_rect_h;

		MP_Rect fill_rect = {};
		fill_rect.x = starting_x;
		fill_rect.y = starting_y;

		float one_round_reload_time = (1.0f / this->reload_per_sec);
		float total_clip_reload_time = (float)this->max_ammo * one_round_reload_time;

		fill_rect.w = (int)(((float)ammo_rect_w * (float)this->max_ammo) * (this->reload_progress / total_clip_reload_time));
		fill_rect.h = ammo_rect_h;

		if (((float)this->ammo / (float)this->max_ammo) > 0.60f) {
			mp_set_render_draw_color(CT_Green);
		}
		else if (((float)this->ammo / (float)this->max_ammo) > 0.30f) {
			mp_set_render_draw_color(CT_Orange);
		}
		else {
			mp_set_render_draw_color(CT_Red);
		}
		mp_render_fill_rect(&fill_rect);

		for (int i = 0; i < this->max_ammo; i++) {
			MP_Rect outline_rect = {};
			outline_rect.x = starting_x + (ammo_rect_w * i);
			outline_rect.y = starting_y;
			outline_rect.w = ammo_rect_w;
			outline_rect.h = ammo_rect_h;

			draw_outline_box(CT_Black, &outline_rect, Globals::DEFAULT_RESOURCE_BAR_OUTLINE, false, false);
		}
	}
	// Draw the attack speed bar
	else {

	}
}

Type_Descriptor weapon_data_type_descriptors[] = {
	FIELD(Weapon_Data, VT_String, weapon_name),
	FIELD(Weapon_Data, VT_Int, weapon_w),
	FIELD(Weapon_Data, VT_Int, weapon_h),
	FIELD(Weapon_Data, VT_Int, damage),
	FIELD(Weapon_Data, VT_Int, max_ammo),
	FIELD(Weapon_Data, VT_Int, attacks_per_sec),
	FIELD(Weapon_Data, VT_Int, reload_per_sec),
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

