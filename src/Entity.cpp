#include "Entity.h"

Player create_player(Image* image, int player_speed) {
	Player result = {};

	result.image = image;
	result.speed = player_speed;

	return result;
}

Arrow create_arrow(Image* image, V2 pos, V2 vel, int speed) {
	Arrow result = {};

	result.image = image;
	result.pos = pos;
	result.vel = vel;
	result.speed = speed;

	return result;
}

void update_arrow(Arrow& arrow, float delta_time) {
	arrow.pos.x += arrow.speed * (arrow.vel.x * delta_time);
	arrow.pos.y += arrow.speed * (arrow.vel.y * delta_time);
}

void draw_arrow(MP_Renderer* renderer, int camera_pos_x, int camera_pos_y, Arrow& arrow) {
	MP_Rect src = {};
	MP_Rect dst = {(int)arrow.pos.x - camera_pos_x, (int)arrow.pos.y - camera_pos_y, 200, 200};
	mp_render_copy_ex(renderer, arrow.image->texture, NULL, &dst, NULL, NULL, SDL_FLIP_NONE);
}
