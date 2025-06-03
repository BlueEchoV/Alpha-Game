#include "Entity.h"

std::unordered_map<std::string, Unit_Data> unit_data_map;
std::unordered_map<std::string, Weapon_Data> weapon_data_map;
std::unordered_map<std::string, Projectile_Data> projectile_data_map;

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
void Weapon::fire_weapon(Game_Data& game_data, Entity_Type et) {
	switch (et) {
	case ET_Player: {
		if (this->can_fire) {
			// Change this to fire weapon
			V2 mouse_cs_pos = get_mouse_position(Globals::renderer->open_gl.window_handle);
			V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, game_data.camera.pos_ws);
			spawn_projectile(game_data, this->projectile_name, game_data.player.rb.pos_ws, mouse_ws_pos);
			Globals::debug_total_arrows++;
			this->can_fire = false;
		}
		break;
	}
	case ET_Allies: {
		break;
	}
	case ET_Enemies: {
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

Player create_player(std::string entity_name, Animation_State as, V2 spawn_pos_ws, int player_speed) {
	Player result = {};

	result.at = create_animation_tracker(entity_name, as, true);

	result.w = Globals::player_width;
	result.h = Globals::player_height;

	result.rb = create_rigid_body(
		{ spawn_pos_ws.x, spawn_pos_ws.y}, 
		player_speed
	);

	/*
	float collider_radius = 32;
	add_collider(&result.rb, { 0,  result.image->sprite_radius / 2 }, collider_radius);
	add_collider(&result.rb, { 0,   	  						 0 }, collider_radius);
	add_collider(&result.rb, { 0, -result.image->sprite_radius / 2 }, collider_radius);
	*/

	return result;
}

void draw_player(Player& p, V2 camera_ws_pos) {
	// Convert the player's position to camera space
	MP_Rect p_draw_rect = {
		(int)p.rb.pos_ws.x - p.w / 2,
		(int)p.rb.pos_ws.y - p.h / 2,
		p.w, p.h};
	// This is the camera space position of the player. Not the world space position.
	V2 p_pos_cs = convert_ws_to_cs({ (float)p_draw_rect.x, (float)p_draw_rect.y }, camera_ws_pos);
	p_draw_rect.x = (int)p_pos_cs.x; 
	p_draw_rect.y = (int)p_pos_cs.y; 
	draw_animation_tracker(&p.at, p_draw_rect, 0);
}

void delete_player(Player* player) {
	delete_weapon(player->weapon);
	*player = {};
}

Unit_Data bad_unit_data = {
	// Unit_Type	 w,   h,   health, damage, speed
	"zombie_male",	75,  75,  100,    10,     10
};

Unit_Data* get_unit_data(std::string unit_type) {
	if (unit_data_map.find(unit_type) != unit_data_map.end()) {
		return &unit_data_map[unit_type];
	}

	return &bad_unit_data;
}

void spawn_unit(std::string unit_name, Animation_State as, Storage<Unit>& storage, std::vector<Handle>& handles, 
	Player* target, V2 spawn_pos) {
	Unit result = {};

	result.unit_name = unit_name;

	Unit_Data* data = get_unit_data(unit_name);
	result.at = create_animation_tracker(unit_name, as, true);
	result.rb = create_rigid_body(spawn_pos, data->speed);

	float collider_radius = (float)data->w / 4;
	add_collider(&result.rb, { 0,  collider_radius / 2 }, collider_radius);
	add_collider(&result.rb, { 0,					 0 }, collider_radius);
	add_collider(&result.rb, { 0, -collider_radius / 2 }, collider_radius);

	result.w = data->w;
	result.h = data->h;
	result.health = data->health;
	result.damage = data->damage;
	result.target = target;
	result.handle = create_handle(storage);
	
	handles.push_back(result.handle);
	storage.storage[result.handle.index] = result;
}

V2 update_unit_position(Unit& unit, float dt) {
	V2 result = {};

	if (unit.dead == false) {
		unit.rb.vel = calculate_normalized_origin_to_target_velocity(unit.target->rb.pos_ws, unit.rb.pos_ws);

		unit.rb.pos_ws.x += (unit.rb.vel.x * unit.rb.speed) * dt;
		unit.rb.pos_ws.y += (unit.rb.vel.y * unit.rb.speed) * dt;
	}

	return result;
};

void update_unit(Unit& unit, float dt) {
	if (unit.destroyed == false) {
		if (unit.dead == false) {
			update_unit_position(unit, dt);

			Animation_Tracker* at = &unit.at;
			float vel_x = unit.rb.vel.x;
			// Change the animation to be facing left or right
			Facing_Direction new_fd = vel_x > 0.0f ? FD_Right : FD_Left;
			if (new_fd != at->fd) {
				at->fd = new_fd;
				change_animation(at, at->entity_name, at->as, at->fd, APS_Speed_Based);
			}
		}
		update_animation_tracker(&unit.at, dt);
	}
}

void draw_unit(Unit& unit, V2 camera_pos) {
	if (unit.destroyed == false) {
		V2 entity_pos_cs = convert_ws_to_cs(unit.rb.pos_ws, { (float)camera_pos.x, (float)camera_pos.y });
		// Center the image on the position of the entity
		MP_Rect dst = { (int)entity_pos_cs.x - unit.w / 2, (int)entity_pos_cs.y - unit.h / 2, unit.w, unit.h };

		draw_animation_tracker(&unit.at, dst, 0);
	}
}

Projectile_Data bad_projectile_data = {
	// name		w	h	damage	speed
	"arrow",	32, 32, 25,		100
};

Projectile_Data get_projectile_data(std::string projectile_name) {
	auto it = projectile_data_map.find(projectile_name);
	if (it == projectile_data_map.end()) {
		return bad_projectile_data;
	}
	return it->second;
}

void spawn_projectile(Game_Data& game_data, std::string projectile_name, V2 origin_ws, V2 target_ws) {
	Projectile result = {};

	Projectile_Data data = get_projectile_data(projectile_name);

	result.rb = create_rigid_body(origin_ws, data.speed);
	V2 vel_normalized = calculate_normalized_origin_to_target_velocity(target_ws, origin_ws);
	result.rb.vel = vel_normalized;

	result.at = create_animation_tracker(projectile_name, AS_No_Animation, true);

	result.w = data.w;
	result.h = data.h;
	result.rb.angle = calculate_facing_direction(result.rb.vel);
	result.handle = create_handle(game_data.projectile_storage);

	Sprite_Sheet* sprite_sheet = get_sprite_sheet(projectile_name);
	float first_sprite_radius = sprite_sheet->sprites[0].image.sprite_radius;
	V2 pos_ls = rotate_point_based_off_angle(result.rb.angle, 0, 0, first_sprite_radius * 0.8f, 0);

	add_collider(&result.rb, pos_ls, first_sprite_radius / 4);

	game_data.projectile_handles.push_back(result.handle);
	game_data.projectile_storage.storage[result.handle.index] = result;
}

void update_projectile(Projectile& projectile, float delta_time) {
	projectile.rb.pos_ws.x += projectile.rb.speed * (projectile.rb.vel.x * delta_time);
	projectile.rb.pos_ws.y += projectile.rb.speed * (projectile.rb.vel.y * delta_time);
}

void draw_projectile(int camera_pos_x, int camera_pos_y, Projectile& projectile) {
	// Draw everything around the camera (converting to camera space)
	V2 entity_pos_cs = convert_ws_to_cs(projectile.rb.pos_ws, { (float)camera_pos_x, 
		(float)camera_pos_y });
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - projectile.w / 2, 
		(int)entity_pos_cs.y - projectile.h / 2, projectile.w, projectile.h };

	draw_animation_tracker(&projectile.at, dst, projectile.rb.angle);
}

Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle) {
	return get_entity_pointer_from_handle(storage, handle);
}

void delete_destroyed_entities_from_handles(Game_Data& game_data) {
	std::erase_if(game_data.enemy_unit_handles, [&game_data](const Handle& enemy_unit_handle) {
		Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
		if (unit != NULL) {
			if (unit->destroyed) {
				delete_handle(game_data.unit_storage, enemy_unit_handle);
				*unit = {};
				return true;
			}
		}
		return false;
	});
	std::erase_if(game_data.projectile_handles, [&game_data](const Handle& projectile_handles) {
		Projectile* proj = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handles);
		if (proj != NULL) {
			if (proj->destroyed) {
				delete_handle(game_data.unit_storage, projectile_handles);
				*proj = {};
				return true;
			}
		}
		return false;
	});
}

Type_Descriptor unit_data_type_descriptors[] = {
	FIELD(Unit_Data, VT_String, unit_name),
	FIELD(Unit_Data, VT_Int, w),
	FIELD(Unit_Data, VT_Int, h),
	FIELD(Unit_Data, VT_Int, health),
	FIELD(Unit_Data, VT_Int, damage),
	FIELD(Unit_Data, VT_Int, speed)
};

void load_unit_data_csv(CSV_Data* data) {
	std::vector<Unit_Data> unit_data;
	unit_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_unit_data_type_descriptors(unit_data_type_descriptors);

	load_csv_data_file(data, (char*)unit_data.data(), safe_unit_data_type_descriptors, sizeof(Unit_Data));

	for (Unit_Data& unit_data_iterator : unit_data) {
		unit_data_map[unit_data_iterator.unit_name] = unit_data_iterator;
	}
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

Type_Descriptor projectile_data_type_descriptors[] = {
	FIELD(Projectile_Data, VT_String, projectile_name),
	FIELD(Projectile_Data, VT_Int, w),
	FIELD(Projectile_Data, VT_Int, h),
	FIELD(Projectile_Data, VT_Int, damage),
	FIELD(Projectile_Data, VT_Int, speed)
};

void load_projectile_data_csv(CSV_Data* data) {
	std::vector<Projectile_Data> projectile_data;

	projectile_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_projectile_data_type_descriptors(projectile_data_type_descriptors);

	load_csv_data_file(data, (char*)projectile_data.data(), safe_projectile_data_type_descriptors, sizeof(Projectile_Data));

	for (Projectile_Data& projectile_data_iterator : projectile_data) {
		projectile_data_map[projectile_data_iterator.projectile_name] = projectile_data_iterator;
	}
}

