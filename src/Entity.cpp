#include "Entity.h"

V2 convert_cs_to_ws(V2 entity_pos, V2 camera_pos) {
	return entity_pos + camera_pos;
}

V2 convert_cs_to_ws(int entity_x, int entity_y, int camera_x, int camera_y) {
	return convert_cs_to_ws({(float)entity_x, (float)entity_y }, {(float)camera_x, (float)camera_y });
}

V2 convert_ws_to_cs(V2 entity_pos, V2 camera_pos) {
	return entity_pos - camera_pos;
}

V2 convert_ws_to_cs(int entity_x, int entity_y, int camera_x, int camera_y) {
	return convert_ws_to_cs({(float)entity_x, (float)entity_y }, {(float)camera_x, (float)camera_y });
}

V2 calculate_origin_to_target_velocity(V2 target, V2 origin) {
	V2 result = {};

	result = target - origin;
	result = normalize(result);

	return result;
}

Rigid_Body create_rigid_body(V2 pos_ws, int speed) {
	Rigid_Body result = {};

	result.pos_ws = pos_ws;
	result.speed = speed;

	return result;
}

Player create_player(Image* image, V2 spawn_pos_ws, int player_speed) {
	Player result = {};

	result.rb = create_rigid_body(spawn_pos_ws, player_speed);
	result.image = image;

	result.w = Globals::player_width;
	result.h = Globals::player_height;

	return result;
}

Unit_Data unit_data[UT_Total_Unit_Types] = {
	// Image_Type			  w,   h,   health, damage, speed
	{IT_Enemy_Clothed_Zombie, 150, 150, 100,    10,     10}
};

Unit_Data* get_unit_data(Unit_Type unit_type) {
	return &unit_data[(int)unit_type];
}

void spawn_unit(Unit_Type unit_type, Storage<Unit>& storage, std::vector<Handle>& handles, 
	Player* target, V2 spawn_pos) {
	Unit result = {};

	Unit_Data* data = get_unit_data(unit_type);
	result.image = get_image(data->image_type);
	result.rb = create_rigid_body(spawn_pos, data->speed);
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

	unit.rb.vel = calculate_origin_to_target_velocity(unit.target->rb.pos_ws, unit.rb.pos_ws);

	unit.rb.pos_ws.x += (unit.rb.vel.x * unit.rb.speed) * dt;
	unit.rb.pos_ws.y += (unit.rb.vel.y * unit.rb.speed) * dt;

	return result;
};

void update_unit(Unit& unit, float dt) {
	update_unit_position(unit, dt);
}

void draw_unit(Unit& unit, V2 camera_pos) {
	V2 entity_pos_cs = convert_ws_to_cs(unit.rb.pos_ws, { (float)camera_pos.x, (float)camera_pos.y});
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - unit.w / 2, (int)entity_pos_cs.y - unit.h / 2, unit.w, unit.h };

	mp_render_copy_ex(unit.image->texture, NULL, &dst, NULL, NULL, SDL_FLIP_NONE);
}

Unit* get_unit_from_handle(Storage<Unit>& storage, Handle handle) {
	return get_entity_pointer_from_handle(storage, handle);
}

void delete_destroyed_entities_from_handles(Game_Data& game_data) {
	std::erase_if(game_data.enemy_unit_handles, [&game_data](const Handle& enemy_unit_handle) {
		Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
		if (unit->is_destroyed) {
			delete_handle(game_data.unit_storage, enemy_unit_handle);
			*unit = {};
			return true;
		}
		return false;
	});
};


Projectile create_projectile(Storage<Projectile>& storage, std::vector<Handle>& projectile_handles, 
	Image* image, V2 pos, V2 vel, int width, int height, int speed) {
	Projectile result = {};

	result.image = image;
	result.pos_ws = pos;
	result.vel = vel;
	result.speed = speed;
	result.w = width;
	result.h = height;
	// NOTE: atan2 returns a range from -180 to 180
	result.angle = (float)atan2((double)vel.x, (double)vel.y);
	log("Arrow Current Angle (Radians): %f", result.angle);
	log("Vel: %f, %f", result.vel.x, result.vel.y);
	// Range from -180 to 180
	result.angle = convert_radians_to_degrees(result.angle);
	result.angle -= 90;
	// NOTE: Convert from range -180 to 180 to 0 to 360
	// -result.angle for counter clockwise
	result.angle = (float)fmod(-result.angle + 360.0, 360.0);
	log("Arrow Current Angle (Degrees): %f", result.angle);
	// Inverted coordinate system
	// result.angle += 180;
	result.handle = create_handle(storage);

	projectile_handles.push_back(result.handle);
	storage.storage[result.handle.index] = result;

	return result;
}

void update_projectile(Projectile& projectile, float delta_time) {
	projectile.pos_ws.x += projectile.speed * (projectile.vel.x * delta_time);
	projectile.pos_ws.y += projectile.speed * (projectile.vel.y * delta_time);
}

void draw_projectile(int camera_pos_x, int camera_pos_y, Projectile& projectile) {
	// Draw everything around the camera (converting to camera space)
	V2 entity_pos_cs = convert_ws_to_cs(projectile.pos_ws, { (float)camera_pos_x, (float)camera_pos_y });
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - projectile.w / 2, (int)entity_pos_cs.y - projectile.h / 2, projectile.w, projectile.h };

	mp_render_copy_ex(projectile.image->texture, NULL, &dst, projectile.angle, NULL, SDL_FLIP_NONE);
}
