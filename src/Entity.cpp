#include "Entity.h"

std::unordered_map<std::string, Unit_Data> unit_data_map;
std::unordered_map<std::string, Weapon_Data> weapon_data_map;
std::unordered_map<std::string, Projectile_Data> projectile_data_map;

Cooldown create_cooldown(float max_cd) {
	Cooldown result = {};

	result.max = max_cd;
	result.current = 0.0f;

	return result;;
}

// Returns true if the cooldown is down, false elsewise
bool check_and_update_cooldown(Cooldown& cd, float delta_time) {
	if (cd.current <= 0.0f) {
		cd.current = cd.max;
		return true;
	}
	else {
		cd.current -= delta_time;
		return false;
	}
}

Health_Bar create_health_bar(int hp, int w, int h, int offset) {
	Health_Bar result = {};

	result.offset = offset;
	result.max_hp = hp;
	result.current_hp = result.max_hp;
	result.w = w;
	result.h = h;

	return result;
}

void draw_health_bar(Color_Type c, Health_Bar& health_bar, V2 pos) {
	// EXAMPLE: Hp = 500 / 1000
	float hp_percent = (float)health_bar.current_hp / (float)health_bar.max_hp;
	clamp(hp_percent, 0.0f, 1.0f);

	// Outline
	mp_set_render_draw_color(CT_Black);
	MP_Rect outline_rect_bottom = {};
	outline_rect_bottom.x = ((int)pos.x - health_bar.w / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_bottom.y = ((int)pos.y - health_bar.h / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_bottom.w = health_bar.w + (Globals::DEFAULT_HEALTH_BAR_OUTLINE * 2);
	outline_rect_bottom.h = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	mp_render_fill_rect(&outline_rect_bottom);

	MP_Rect outline_rect_top = {};
	outline_rect_top.x = ((int)pos.x - health_bar.w / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_top.y = ((int)pos.y + health_bar.h / 2);
	outline_rect_top.w = health_bar.w + (Globals::DEFAULT_HEALTH_BAR_OUTLINE * 2);
	outline_rect_top.h = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	mp_render_fill_rect(&outline_rect_top);

	MP_Rect outline_rect_left = {};
	outline_rect_left.x = ((int)pos.x - health_bar.w / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_left.y = ((int)pos.y - health_bar.h / 2);
	outline_rect_left.w = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_left.h = health_bar.h;
	mp_render_fill_rect(&outline_rect_left);

	MP_Rect outline_rect_right = {};
	outline_rect_right.x = ((int)pos.x + health_bar.w / 2);
	outline_rect_right.y = ((int)pos.y - health_bar.h / 2);
	outline_rect_right.w = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_right.h = health_bar.h;
	mp_render_fill_rect(&outline_rect_right);

	// Green section
	MP_Rect green_rect = {};
	green_rect.x = (int)pos.x - health_bar.w / 2;
	green_rect.y = (int)pos.y - health_bar.h / 2;
	green_rect.w = (int)((float)health_bar.w * hp_percent);
	green_rect.w = (int)lerp(0.0f, (float)health_bar.w, hp_percent);
	green_rect.h = health_bar.h;
	mp_set_render_draw_color(c);
	mp_render_fill_rect(&green_rect);

	// Red section
	MP_Rect red_rect = {};
	red_rect.x = green_rect.x + green_rect.w;
	red_rect.y = (int)pos.y - health_bar.h / 2;
	red_rect.w = health_bar.w - green_rect.w;
	red_rect.h = health_bar.h;
	mp_set_render_draw_color(CT_Dark_Grey);
	mp_render_fill_rect(&red_rect);
}

void draw_faction_health_bar(Faction faction, Health_Bar& health_bar, V2 pos) {
	Color_Type c;

	switch (faction) {
	case F_Player: {
		c = CT_Dark_Green;
		break;
	}
	case F_Allies: {
		c = CT_Dark_Blue;
		break;
	}
	case F_Enemies: {
		c = CT_Red;
		break;
	}
	default: {
		c = CT_Blue;
		break;
	}
	}

	draw_health_bar(c, health_bar, pos);
}

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
void Weapon::fire_weapon(Game_Data& game_data, Faction faction) {
	switch (faction) {
	case F_Player: {
		if (this->can_fire) {
			// Change this to fire weapon
			V2 mouse_cs_pos = get_mouse_position(Globals::renderer->open_gl.window_handle);
			V2 mouse_ws_pos = convert_cs_to_ws(mouse_cs_pos, game_data.camera.pos_ws);
			spawn_projectile(game_data, this->projectile_name, this->damage, game_data.player.rb.pos_ws, mouse_ws_pos);
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

Player_Data bad_player_data = {
	// character_name		hp		speed		damage		w		h
	"player_1",				100,	200,		100,		100,	100 	
};

std::unordered_map<std::string, Player_Data> player_data_map;

Player_Data get_player_data(std::string character_name) {
	auto it = player_data_map.find(character_name);
	if (it != player_data_map.end()) {
		return it->second;
	}

	return bad_player_data;
};

Player create_player(std::string character_name, V2 spawn_pos_ws) {
	Player result = {};

	result.faction = F_Player;
	result.character_name = character_name;

	Player_Data data = get_player_data(character_name);

	// NOTE: AS_Idle is the default starting animation for the player
	result.at = create_animation_tracker(character_name, AS_Idle, true);

	result.w = data.h;
	result.h = data.w;

	result.rb = create_rigid_body(
		{ spawn_pos_ws.x, spawn_pos_ws.y}, 
		data.base_speed
	);

	result.health_bar = create_health_bar(
		data.hp, 
		Globals::DEFAULT_HEALTH_BAR_WIDTH, 
		Globals::DEFAULT_HEALTH_BAR_HEIGHT, 
		data.h / 2 + Globals::DEFAULT_HEALTH_BAR_HEIGHT
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
	// WE ARE DRAWING THE PLAYER RELATIVE TO THE CAMERA.
	MP_Rect p_draw_rect = {
		(int)p.rb.pos_ws.x - p.w / 2,
		(int)p.rb.pos_ws.y - p.h / 2,
		p.w, p.h};
	V2 p_pos_cs = convert_ws_to_cs({ (float)p_draw_rect.x, (float)p_draw_rect.y }, camera_ws_pos);
	p_draw_rect.x = (int)p_pos_cs.x; 
	p_draw_rect.y = (int)p_pos_cs.y; 
	draw_animation_tracker(&p.at, p_draw_rect, 0);

	V2 health_bar_cs_pos = p.rb.pos_ws;
	health_bar_cs_pos.y += p.health_bar.offset;
	health_bar_cs_pos = convert_ws_to_cs(health_bar_cs_pos, camera_ws_pos);
	draw_faction_health_bar(p.faction, p.health_bar, health_bar_cs_pos);
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

void spawn_unit(Faction faction, std::string unit_name, Animation_State as, Storage<Unit>& storage, std::vector<Handle>& handles, 
	Player* target, V2 spawn_pos) {
	Unit result = {};

	result.faction = faction;
	result.unit_name = unit_name;

	Unit_Data* data = get_unit_data(unit_name);
	result.health_bar = create_health_bar(
		data->health, 
		Globals::DEFAULT_HEALTH_BAR_WIDTH, 
		Globals::DEFAULT_HEALTH_BAR_HEIGHT, 
		data->h / 2 + Globals::DEFAULT_HEALTH_BAR_HEIGHT
	);
	result.at = create_animation_tracker(unit_name, as, true);
	result.rb = create_rigid_body(spawn_pos, data->speed);

	float collider_radius = (float)data->w / 4;
	add_collider(&result.rb, { 0,  collider_radius / 2 }, collider_radius);
	add_collider(&result.rb, { 0,					 0 }, collider_radius);
	add_collider(&result.rb, { 0, -collider_radius / 2 }, collider_radius);

	result.w = data->w;
	result.h = data->h;
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

		unit.rb.pos_ws.x += (unit.rb.vel.x * unit.rb.current_speed) * dt;
		unit.rb.pos_ws.y += (unit.rb.vel.y * unit.rb.current_speed) * dt;
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

		if (!unit.dead) {
			V2 health_bar_cs_pos = entity_pos_cs;
			health_bar_cs_pos.y += unit.health_bar.offset;
			draw_faction_health_bar(unit.faction, unit.health_bar, health_bar_cs_pos);
		}
	}
}

Projectile_Data bad_projectile_data = {
	// name		w	h	speed
	"arrow",	32, 32, 100
};

Projectile_Data get_projectile_data(std::string projectile_name) {
	auto it = projectile_data_map.find(projectile_name);
	if (it == projectile_data_map.end()) {
		return bad_projectile_data;
	}
	return it->second;
}

void spawn_projectile(Game_Data& game_data, std::string projectile_name, int damage, V2 origin_ws, V2 target_ws) {
	Projectile result = {};
	result.damage = damage;

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
	projectile.rb.pos_ws.x += projectile.rb.current_speed * (projectile.rb.vel.x * delta_time);
	projectile.rb.pos_ws.y += projectile.rb.current_speed * (projectile.rb.vel.y * delta_time);
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

Horde_Data bad_horde_data = {
	// level		total_zombies		spawning_cd
	   1,			20,					0.5f
};

std::unordered_map<std::string, Horde_Data> horde_data_map;

Horde_Data get_horde_data(Horde_Type horde_type) {
	Horde_Data result = {};

	if (horde_type == HT_Not_Specified) {
		return bad_horde_data;
	}
	return bad_horde_data;
};

// Call this at the end of every day?
Horde create_horde(Faction faction, Horde_Type ht, MP_Rect rect) {
	Horde result;

	Horde_Data data = get_horde_data(ht);

	result.faction = faction;
	result.level = data.level;
	result.spawning_cd = create_cooldown(data.max_spawning_cd);
	result.spawn_region_ws = rect;
	result.begin_spawning = false;
	result.total_to_spawn = data.total_to_spawn;
	result.total_spawned = 0;

	return result;
}

void spawn_and_update_horde(Game_Data& game_data, float delta_time) {
	if (game_data.current_horde.begin_spawning) {
		if (game_data.current_horde.total_spawned < game_data.current_horde.total_to_spawn) {
			if (check_and_update_cooldown(game_data.current_horde.spawning_cd, delta_time)) {
				if (game_data.current_horde.faction == F_Enemies) {
					V2 random_pos_ws = {};
					MP_Rect spawn_region_ws = game_data.current_horde.spawn_region_ws;
					random_pos_ws.x = (float)((int)spawn_region_ws.x + (rand() % spawn_region_ws.w));
					random_pos_ws.y = (float)((int)spawn_region_ws.y + (rand() % spawn_region_ws.h));
					spawn_unit(
						game_data.current_horde.faction,
						"zombie_woman",
						AS_Running,
						game_data.unit_storage,
						game_data.enemy_unit_handles,
						&game_data.player,
						random_pos_ws
					);
					game_data.current_horde.spawning_cd.current++;
				}
			}
		}
	}
}

// NOTE: Draw diagnal lines? Or dotted lines?
void draw_horde_spawn_region(Color_Type c, Horde& horde, V2 camera_pos) {
	V2 pos_ws = {(float)horde.spawn_region_ws.x, (float)horde.spawn_region_ws.y};
	V2 pos_cs = convert_ws_to_cs(pos_ws, camera_pos);
	MP_Rect cs_rect = { (int)pos_cs.x, (int)pos_cs.y, horde.spawn_region_ws.w, horde.spawn_region_ws.h };

	mp_set_render_draw_color(c);
	mp_render_draw_rect(&cs_rect);
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

