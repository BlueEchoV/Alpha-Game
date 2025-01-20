#include "Entity.h"

V2 convert_to_camera_space(V2 entity_pos, V2 camera_pos) {
	return camera_pos - entity_pos;
}

V2 convert_to_camera_space(int entity_x, int entity_y, int camera_x, int camera_y) {
	return convert_to_camera_space({(float)entity_x, (float)entity_y }, { (float)camera_x, (float)camera_y });
}

Player create_player(Image* image, int player_speed) {
	Player result = {};

	result.image = image;
	result.speed = player_speed;

	return result;
}

Arrow create_arrow(Image* image, V2 pos, V2 vel, int speed) {
	Arrow result = {};

	result.image = image;
	result.pos_ws = pos;
	result.vel = vel;
	result.speed = speed;

	return result;
}

void update_arrow(Arrow& arrow, float delta_time) {
	arrow.pos_ws.x += arrow.speed * (arrow.vel.x * delta_time);
	arrow.pos_ws.y += arrow.speed * (arrow.vel.y * delta_time);
}

void draw_arrow(MP_Renderer* renderer, int camera_pos_x, int camera_pos_y, Arrow& arrow) {
	// Draw everything around the camera (converting to camera space)
	V2 entity_pos_cs = convert_to_camera_space({ (float)camera_pos_x, (float)camera_pos_y }, arrow.pos_ws);
	MP_Rect dst = { (int)entity_pos_cs.x, (int)entity_pos_cs.y, 200, 200 };
	mp_render_copy_ex(renderer, arrow.image->texture, NULL, &dst, NULL, NULL, SDL_FLIP_NONE);
}
