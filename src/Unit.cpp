#include "Unit.h"

std::unordered_map<std::string, Unit_Data> unit_data_map;

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

void spawn_unit(Faction faction, std::string_view unit_name, Animation_State starting_as, Animation_Play_Speed starting_aps, Animation_Mode starting_am, 
	Storage<Unit>& storage, std::vector<Handle>& handles, Player* target, V2 spawn_pos) {
	Unit result = {};

	result.faction = faction;
	result.unit_name = unit_name;
	result.us = US_Moving;

	Unit_Data* data = get_unit_data(std::string(unit_name));
	result.health_bar = create_health_bar(
		data->health, 
		Globals::DEFAULT_HEALTH_BAR_WIDTH, 
		Globals::DEFAULT_HEALTH_BAR_HEIGHT, 
		data->h / 2 + Globals::DEFAULT_HEALTH_BAR_HEIGHT
	);

	result.at = create_animation_tracker(ATT_Direction_8_Atlas, unit_name, starting_as, starting_aps, starting_am, true);
	result.rb = create_rigid_body(spawn_pos, data->speed);

	result.w = data->w;
	result.h = data->h;
	
	float collider_radius = (float)result.h / 6.0f;
	add_collider(&result.rb, { 0, (float)result.h / 4.0f  }, collider_radius);
	add_collider(&result.rb, { 0, 		      0 }, collider_radius);
	add_collider(&result.rb, { 0, (float)-result.h / 4.0f }, collider_radius);

	result.can_attack = true;
	result.damage = data->damage;
	result.attack_cd.max = (1.0f / data->attacks_per_sec);
	result.attack_cd.current = result.attack_cd.max;
	result.target = target;
	result.handle = create_handle(storage);
	
	handles.push_back(result.handle);
	storage.storage[result.handle.index] = result;
}

void kill_unit(Unit& unit, int& active_enemy_units_counter) {
	if (!unit.dead) {
		change_animation_tracker(&unit.at, unit.unit_name, AS_Dying, APS_Fast, AM_Animate_Once, false, unit.rb.vel);
		unit.us = US_Dying;
		unit.dead = true;
		active_enemy_units_counter--;
	}
}

V2 update_unit_position(Unit& unit, float dt) {
	V2 result = {};

	if (unit.dead == false && unit.us == US_Moving) {
		unit.rb.vel = calculate_normalized_origin_to_target_velocity(unit.target->rb.pos_ws, unit.rb.pos_ws);

		unit.rb.pos_ws.x += (unit.rb.vel.x * unit.rb.current_speed) * dt;
		unit.rb.pos_ws.y += (unit.rb.vel.y * unit.rb.current_speed) * dt;
	}

	return result;
};

void update_unit(Player& p, Unit& unit, float dt) {
	float speed = (float)unit.rb.base_speed;
	bool stop_moving = false;

	// Change the animation tracker
	switch (unit.us) {
	case US_Idle: {
		change_animation_tracker(&unit.at, unit.unit_name, AS_Walking, APS_Speed_Based, AM_Animate_Looping, false, unit.rb.vel);
		break;
	}
	case US_Moving: {
		// change_animation_state to either walking or running
		if (check_rb_collision(&p.rb, &unit.rb)) {
			if (unit.attack_cd.current <= 0.0f && unit.can_attack) {
				unit.us = US_Attacking;
				unit.damage_applied = false;
				change_animation_tracker(&unit.at, unit.unit_name, AS_Attacking, APS_Fast, AM_Animate_Once, false, unit.rb.vel);
			}
			// Don't move if we are in range of target
			// TODO: Add idle animation here? Or just stand still with static first frame
			stop_moving = true;
		} else {
			stop_moving = false;
			change_animation_tracker(&unit.at, unit.unit_name, AS_Walking, APS_Speed_Based, AM_Animate_Looping, false, unit.rb.vel);
		}
		break;
	}
	case US_Attacking: {
		// 1: We need to check if we are in the attack frame of the unit and if damage has yet been applied this cycle (multiple frames
		// per cycle.
	if (unit.at.current_frame_index == unit.attack_hit_frame && !unit.damage_applied && cooldown_ready(unit.attack_cd)) {
			// 2: Next, we check if we are colliding with the player this frame.
			if (check_rb_collision(&p.rb, &unit.rb)) {
				// 3: If we are colliding, we apply the damage to the player and check if the player is dead or not
				p.health_bar.current_hp -= unit.damage;
				if (p.health_bar.current_hp <= 0) {
					p.dead = true;
				}
				unit.damage_applied = true;
				trigger_cooldown(unit.attack_cd);
			}
		}
		// 4: We have hit the end of the frame. Set it back to moving.
		if (unit.at.mode == AM_Static_Last_Frame) {
			unit.us = US_Moving;
			unit.damage_applied = false;
			unit.attack_cd.current = unit.attack_cd.max;
			change_animation_tracker(&unit.at, unit.unit_name, AS_Walking, APS_Speed_Based, AM_Animate_Looping, false, unit.rb.vel);
		}
		break;
	}
	case US_Dying: {
		// Set when the unit dies
		break;
	}
	}

	update_animation_tracker(&unit.at, dt, speed);

	if (unit.destroyed == false) {
		if (unit.dead == false) {
			if (stop_moving == false) {
				update_unit_position(unit, dt);
			}

			// change_animation_direction_8(at, at->entity_name, at->as, unit.rb.vel, APS_Fast);
			update_cooldown(unit.attack_cd, dt);
		}
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

void draw_unit_outlined(Unit& unit, V2 camera_pos, Color_Type outline_color, float outline_thickness) {
	if (unit.destroyed == false) {
		V2 entity_pos_cs = convert_ws_to_cs(unit.rb.pos_ws, { (float)camera_pos.x, (float)camera_pos.y });
		// Center the image on the position of the entity
		MP_Rect dst = { (int)entity_pos_cs.x - unit.w / 2, (int)entity_pos_cs.y - unit.h / 2, unit.w, unit.h };

		draw_animation_tracker_outlined(&unit.at, dst, 0, outline_color, outline_thickness);

		if (!unit.dead) {
			V2 health_bar_cs_pos = entity_pos_cs;
			health_bar_cs_pos.y += unit.health_bar.offset;
			draw_faction_health_bar(unit.faction, unit.health_bar, health_bar_cs_pos);
		}
	}
}

Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle) {
	return get_entity_pointer_from_handle(storage, handle);
}

Type_Descriptor unit_data_type_descriptors[] = {
	FIELD(Unit_Data, VT_String, unit_name),
	FIELD(Unit_Data, VT_Int, w),
	FIELD(Unit_Data, VT_Int, h),
	FIELD(Unit_Data, VT_Int, health),
	FIELD(Unit_Data, VT_Int, damage),
	FIELD(Unit_Data, VT_Float, attacks_per_sec),
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
