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
	result.at = create_animation_tracker(ATT_Direction_8, unit_name, as, true);
	change_animation_tracker(&result.at, result.at.entity_name, result.at.as, APS_Fast, false, result.rb.vel);
	result.rb = create_rigid_body(spawn_pos, data->speed);

	result.w = data->w;
	result.h = data->h;
	
	float collider_radius = (float)result.h / 6.0f;
	add_collider(&result.rb, { 0, (float)result.h / 4.0f  }, collider_radius);
	add_collider(&result.rb, { 0, 		      0 }, collider_radius);
	add_collider(&result.rb, { 0, (float)-result.h / 4.0f }, collider_radius);

	result.can_attack = true;
	result.damage = data->damage;
	result.attack_cd.max = (1000.0f / data->attacks_per_sec);
	result.attack_cd.current = result.attack_cd.max;
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
			REF(at);

			// change_animation_direction_8(at, at->entity_name, at->as, unit.rb.vel, APS_Fast);
			if (check_and_update_cooldown(unit.attack_cd, dt)) {
				unit.can_attack = true;
			}
			else {
				unit.can_attack = false;
			}
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
