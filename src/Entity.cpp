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

void draw_circle_ws(Color_Type c, V2 center_pos_ws, int radius, float total_lines) {
	V2 pos_cs = center_pos_ws;

	mp_set_render_draw_color(c);

	float increment_angle = 360.0f / total_lines;
	float current_degrees = 0;
	for (int i = 0; i < total_lines; i++) {
		float current_radians = convert_degrees_to_radians(current_degrees);
		float force_x = cos(current_radians);
		float force_y = sin(current_radians);
		float x1 = force_x * radius;
		float y1 = force_y * radius;
		current_degrees += increment_angle;

		current_radians = convert_degrees_to_radians(current_degrees);
		force_x = cos(current_radians);
		force_y = sin(current_radians);
		float x2 = force_x * radius;
		float y2 = force_y * radius;

		x1 += pos_cs.x;
		y1 += pos_cs.y;
		x2 += pos_cs.x;
		y2 += pos_cs.y;

		mp_render_draw_line((int)x1, (int)y1, (int)x2, (int)y2);
	}
}

void draw_circle_cs(Color_Type c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines) {
	V2 pos_cs = convert_ws_to_cs(center_pos_ws, camera_pos);

	draw_circle_ws(c, pos_cs, radius, total_lines);
}

void add_collider(Rigid_Body* rb, V2 pos_ls, float radius) {
	if (rb->num_colliders < Globals::MAX_COLLIDERS) {
		Collider collider = {};

		collider.pos_ls = pos_ls;
		collider.radius = radius;

		rb->colliders[rb->num_colliders] = collider;
		rb->num_colliders++; 
	}
}

void draw_colliders(Rigid_Body* rb, V2 camera_pos) {
	for (int i = 0; i < rb->num_colliders; i++) {
		Collider* c = &rb->colliders[i];
		if (c == NULL) {
			continue;
		}

		V2 collider_ws_pos = rb->pos_ws + c->pos_ls;
		draw_circle_cs(CT_Green, collider_ws_pos, camera_pos, (int)c->radius, 20);
	}
}

Rigid_Body create_rigid_body(V2 pos_ws, int speed) {
	Rigid_Body result = {};

	result.pos_ws = pos_ws;
	result.speed = speed;
	result.angle = 0.0f;

	return result;
}

Player create_player(Image* image, V2 spawn_pos_ws, int player_speed, int fire_rate_per_sec) {
	Player result = {};

	result.image = image;

	result.w = Globals::player_width;
	result.h = Globals::player_height;

	result.rb = create_rigid_body(
		{ spawn_pos_ws.x, spawn_pos_ws.y}, 
		player_speed
	);

	result.fire_rate = fire_rate_per_sec;

	float collider_radius = 32;
	add_collider(&result.rb, { 0,  result.image->sprite_radius / 2 }, collider_radius);
	add_collider(&result.rb, { 0,   	  						 0 }, collider_radius);
	add_collider(&result.rb, { 0, -result.image->sprite_radius / 2 }, collider_radius);

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
	mp_render_copy_ex(p.image->texture, NULL, &p_draw_rect, p.rb.angle, NULL, SDL_FLIP_NONE);
}

Unit_Data unit_data[UT_Total_Unit_Types] = {
	// Image_Type			  w,   h,   health, damage, speed
	{IT_Enemy_Clothed_Zombie, 150, 150, 100,    10,     10}
};

Unit_Data* get_unit_data(Unit_Type unit_type) {
	return &unit_data[(int)unit_type];
}

void spawn_unit(Unit_Type unit_type, Storage<Unit>& storage, std::vector<Handle>& handles, Player* target, V2 spawn_pos) {
	Unit result = {};

	Unit_Data* data = get_unit_data(unit_type);
	result.image = get_image(data->image_type);
	result.rb = create_rigid_body(spawn_pos, data->speed);

	float collider_radius = result.image->sprite_radius;
	add_collider(&result.rb, { 0,  result.image->sprite_radius / 2 }, collider_radius);
	add_collider(&result.rb, { 0,   	  						 0 }, collider_radius);
	add_collider(&result.rb, { 0, -result.image->sprite_radius / 2 }, collider_radius);

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

	unit.rb.vel = calculate_normalized_origin_to_target_velocity(unit.target->rb.pos_ws, unit.rb.pos_ws);

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

	mp_render_copy_ex(unit.image->texture, NULL, &dst, unit.rb.angle, NULL, SDL_FLIP_NONE);
}

Projectile_Data projectile_data = {
	"Arrow", IT_Arrow_1, 32, 32, 100 
};

void spawn_projectile(Game_Data& game_data, V2 origin_ws, V2 target_ws) {
	Projectile result = {};

	Projectile_Data data = projectile_data;

	result.rb = create_rigid_body(origin_ws, data.speed);
	V2 vel_normalized = calculate_normalized_origin_to_target_velocity(target_ws, origin_ws);
	result.rb.vel = vel_normalized;

	result.image = get_image(data.it);
	result.w = data.w;
	result.h = data.h;
	result.angle = calculate_facing_direction(result.rb.vel);
	result.handle = create_handle(game_data.projectile_storage);

	add_collider(&result.rb, { result.image->sprite_radius * 0.8f }, result.image->sprite_radius / 4);

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

	mp_render_copy_ex(projectile.image->texture, NULL, &dst, projectile.angle, NULL, SDL_FLIP_NONE);
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
};
