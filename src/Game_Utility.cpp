#include "Game_Utility.h"

// Camera's position is relative to the player
void update_camera(Camera& camera, V2 player_pos) {
	// The camera is offset from the player
	camera.pos_ws.x = (player_pos.x - (Globals::playground_area_w / 2.0f));
	camera.pos_ws.y = (player_pos.y - (Globals::playground_area_h / 2.0f));

	camera.w = Globals::playground_area_w;
	camera.h = Globals::playground_area_h;
}

// NOTE: The camera is based off the player, and the player is draw relative to the camera.
Camera create_camera(V2 player_pos_ws) {
	Camera result = {};

	update_camera(result, player_pos_ws);

	return result;
}

Cooldown create_cooldown(float max_cd) {
	Cooldown result = {};

	result.max = max_cd;
	result.current = 0.0f;

	return result;;
}

// Returns true if the cooldown is down, false elsewise
bool check_and_update_cooldown(Cooldown& cd, float delta_time) {
	if (cd.current <= 0.0f) {
		cd.current = cd.max;
		return true;
	}
	else {
		cd.current -= delta_time;
		return false;
	}
}

Health_Bar create_health_bar(int hp, int w, int h, int offset) {
	Health_Bar result = {};

	result.offset = offset;
	result.max_hp = hp;
	result.current_hp = result.max_hp;
	result.w = w;
	result.h = h;

	return result;
}

void draw_health_bar(Color_Type c, Health_Bar& health_bar, V2 pos) {
	// EXAMPLE: Hp = 500 / 1000
	float hp_percent = (float)health_bar.current_hp / (float)health_bar.max_hp;
	clamp(hp_percent, 0.0f, 1.0f);

	// Outline
	mp_set_render_draw_color(CT_Black);
	MP_Rect outline_rect_bottom = {};
	outline_rect_bottom.x = ((int)pos.x - health_bar.w / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_bottom.y = ((int)pos.y - health_bar.h / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_bottom.w = health_bar.w + (Globals::DEFAULT_HEALTH_BAR_OUTLINE * 2);
	outline_rect_bottom.h = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	mp_render_fill_rect(&outline_rect_bottom);

	MP_Rect outline_rect_top = {};
	outline_rect_top.x = ((int)pos.x - health_bar.w / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_top.y = ((int)pos.y + health_bar.h / 2);
	outline_rect_top.w = health_bar.w + (Globals::DEFAULT_HEALTH_BAR_OUTLINE * 2);
	outline_rect_top.h = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	mp_render_fill_rect(&outline_rect_top);

	MP_Rect outline_rect_left = {};
	outline_rect_left.x = ((int)pos.x - health_bar.w / 2) - Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_left.y = ((int)pos.y - health_bar.h / 2);
	outline_rect_left.w = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_left.h = health_bar.h;
	mp_render_fill_rect(&outline_rect_left);

	MP_Rect outline_rect_right = {};
	outline_rect_right.x = ((int)pos.x + health_bar.w / 2);
	outline_rect_right.y = ((int)pos.y - health_bar.h / 2);
	outline_rect_right.w = Globals::DEFAULT_HEALTH_BAR_OUTLINE;
	outline_rect_right.h = health_bar.h;
	mp_render_fill_rect(&outline_rect_right);

	// Green section
	MP_Rect green_rect = {};
	green_rect.x = (int)pos.x - health_bar.w / 2;
	green_rect.y = (int)pos.y - health_bar.h / 2;
	green_rect.w = (int)((float)health_bar.w * hp_percent);
	green_rect.w = (int)lerp(0.0f, (float)health_bar.w, hp_percent);
	green_rect.h = health_bar.h;
	mp_set_render_draw_color(c);
	mp_render_fill_rect(&green_rect);

	// Red section
	MP_Rect red_rect = {};
	red_rect.x = green_rect.x + green_rect.w;
	red_rect.y = (int)pos.y - health_bar.h / 2;
	red_rect.w = health_bar.w - green_rect.w;
	red_rect.h = health_bar.h;
	mp_set_render_draw_color(CT_Dark_Grey);
	mp_render_fill_rect(&red_rect);
}

void draw_faction_health_bar(Faction faction, Health_Bar& health_bar, V2 pos) {
	Color_Type c;

	switch (faction) {
	case F_Player: {
		c = CT_Dark_Green;
		break;
	}
	case F_Allies: {
		c = CT_Dark_Blue;
		break;
	}
	case F_Enemies: {
		c = CT_Red;
		break;
	}
	default: {
		c = CT_Blue;
		break;
	}
	}

	draw_health_bar(c, health_bar, pos);
}


