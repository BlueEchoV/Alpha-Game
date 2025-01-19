#include "Entity.h"

Player create_player(Image* image, int player_speed) {
	Player result = {};

	result.image = image;
	result.speed = player_speed;

	return result;
}

Arrow create_arrow(Image* image, V2 pos, int speed) {
	Arrow result = {};

	result.image = image;
	result.pos = pos;
	result.speed = speed;

	return result;
}

void update_arrow(Arrow& arrow) {
	REF(arrow);
}

void draw_arrow(MP_Renderer* renderer, Arrow& arrow) {
	MP_Rect src = {};
	MP_Rect dst = {(int)arrow.pos.x, (int)arrow.pos.y, 200, 200};
	mp_render_copy_ex(renderer, arrow.image->texture, NULL, &dst, NULL, NULL, SDL_FLIP_NONE);
}
