#include "Entity.h"

V2 convert_to_camera_space(V2 entity_pos, V2 camera_pos) {
	return entity_pos - camera_pos;
}

V2 convert_to_camera_space(int entity_x, int entity_y, int camera_x, int camera_y) {
	return convert_to_camera_space({(float)entity_x, (float)entity_y }, {(float)camera_x, (float)camera_y });
}

Player create_player(Image* image, int player_speed) {
	Player result = {};

	result.image = image;
	result.speed = player_speed;

	return result;
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

void draw_arrow(MP_Renderer* renderer, int camera_pos_x, int camera_pos_y, Arrow& arrow) {
	// Draw everything around the camera (converting to camera space)
	V2 entity_pos_cs = convert_to_camera_space(arrow.pos_ws, { (float)camera_pos_x, (float)camera_pos_y });
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - arrow.w / 2, (int)entity_pos_cs.y - arrow.h / 2, arrow.w, arrow.h };

	std::string arrow_angle = std::to_string(arrow.angle);
	draw_string(renderer, arrow_angle.c_str(), dst.x, dst.y);

	mp_render_copy_ex(renderer, arrow.image->texture, NULL, &dst, arrow.angle, NULL, SDL_FLIP_NONE);
}
