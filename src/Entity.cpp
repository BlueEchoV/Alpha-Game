#include "Entity.h"

Player create_player(Image* image, int player_speed) {
	Player result = {};

	result.image = image;
	result.speed = player_speed;

	return result;
}
