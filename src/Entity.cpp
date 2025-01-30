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

void spawn_zombie(Storage<Zombie>& storage, std::vector<Handle>& handles, Image* image, Player* target, V2 spawn_pos,
	int width, int height, int speed, int health, int damage) {
	Zombie result = {};

	result.image = image;
	result.rb = create_rigid_body(spawn_pos, speed);
	result.w = width;
	result.h = height;
	result.health = health;
	result.damage = damage;
	result.target = target;
	result.handle = create_handle(storage);
	
	handles.push_back(result.handle);
	storage.storage[result.handle.index] = result;
}

V2 update_zombie_position(Zombie& zombie, float dt) {
	V2 result = {};

	zombie.rb.vel = calculate_origin_to_target_velocity(zombie.target->rb.pos_ws, zombie.rb.pos_ws);

	zombie.rb.pos_ws.x += (zombie.rb.vel.x * zombie.rb.speed) * dt;
	zombie.rb.pos_ws.y += (zombie.rb.vel.y * zombie.rb.speed) * dt;

	return result;
};

void update_zombie(Zombie& zombie, float dt) {
	update_zombie_position(zombie, dt);
}

void draw_zombie(Zombie& zombie, V2 camera_pos) {
	V2 entity_pos_cs = convert_ws_to_cs(zombie.rb.pos_ws, { (float)camera_pos.x, (float)camera_pos.y});
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - zombie.w / 2, (int)entity_pos_cs.y - zombie.h / 2, zombie.w, zombie.h };

	mp_render_copy_ex(zombie.image->texture, NULL, &dst, NULL, NULL, SDL_FLIP_NONE);
}

Zombie* get_zombie_from_handle(Storage<Zombie>& storage, Handle handle) {
	return get_entity_pointer_from_handle(storage, handle);
}

Arrow create_arrow(Image* image, V2 pos, V2 vel, int width, int height, int speed) {
	Arrow result = {};

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

	return result;
}

void update_arrow(Arrow& arrow, float delta_time) {
	arrow.pos_ws.x += arrow.speed * (arrow.vel.x * delta_time);
	arrow.pos_ws.y += arrow.speed * (arrow.vel.y * delta_time);
}

void draw_arrow(int camera_pos_x, int camera_pos_y, Arrow& arrow) {
	// Draw everything around the camera (converting to camera space)
	V2 entity_pos_cs = convert_ws_to_cs(arrow.pos_ws, { (float)camera_pos_x, (float)camera_pos_y });
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - arrow.w / 2, (int)entity_pos_cs.y - arrow.h / 2, arrow.w, arrow.h };

	mp_render_copy_ex(arrow.image->texture, NULL, &dst, arrow.angle, NULL, SDL_FLIP_NONE);
}
