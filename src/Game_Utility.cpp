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
		if (cd.current < 0.0f) {
			cd.current = 0.0f;
		}
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

// The origin is the center 
// The width and height input is the total weight of the outline. The outlines go inward.
// The function outlines the area with four rectangles
void draw_outline_box(Color_Type c, MP_Rect* rect, int outline_thickness, bool exclude_outline_from_w_and_h, bool center) {
	mp_set_render_draw_color(c);

	MP_Rect final_rect = *rect;
	if (center) {
		final_rect.x -= final_rect.w / 2;
		final_rect.y -= final_rect.h / 2;
	}

	// Default origin is bottom left and includes outline in the box
	MP_Rect n = {};
	n.x = final_rect.x;
	n.y = (final_rect.y + final_rect.h) - outline_thickness;
	n.w = final_rect.w;
	n.h = outline_thickness;

	MP_Rect s = {};
	s.x = final_rect.x;
	s.y = final_rect.y;
	s.w = final_rect.w;
	s.h = outline_thickness;

	MP_Rect e = {};
	e.x = (final_rect.x + final_rect.w) - outline_thickness;
	e.y = final_rect.y;
	e.w = outline_thickness;
	e.h = final_rect.h;

	MP_Rect w = {};
	w.x = final_rect.x;
	w.y = final_rect.y;
	w.w = outline_thickness;
	w.h = final_rect.h;

	if (exclude_outline_from_w_and_h) {
		n.y += outline_thickness;

		s.y -= outline_thickness;

		e.x += outline_thickness;
		e.y -= outline_thickness;
		e.h += outline_thickness * 2;

		w.x -= outline_thickness;
		w.y -= outline_thickness;
		w.h += outline_thickness * 2;
	}

	mp_render_fill_rect(&s);
	mp_render_fill_rect(&n);
	mp_render_fill_rect(&w);
	mp_render_fill_rect(&e);
}

void draw_health_bar(Color_Type c, Health_Bar& health_bar, V2 pos) {
	// EXAMPLE: Hp = 500 / 1000
	float hp_percent = (float)health_bar.current_hp / (float)health_bar.max_hp;
	clamp(hp_percent, 0.0f, 1.0f);

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

	MP_Rect final_outline_rect = {};
	final_outline_rect.x = (int)pos.x;
	final_outline_rect.y = (int)pos.y;
	final_outline_rect.w = health_bar.w;
	final_outline_rect.h = health_bar.h;
	draw_outline_box(CT_Black, &final_outline_rect, 1, true , true);
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


