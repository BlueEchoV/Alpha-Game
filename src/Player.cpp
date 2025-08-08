#include "Player.h"

Player_Data bad_player_data = {
	// character_name		hp		speed		damage		w		h
	"player_1",				100,	200,		100,		100,	100 	
};

std::unordered_map<std::string, Player_Data> player_data_map;

Player_Data get_player_data(std::string_view character_name) {
	auto it = player_data_map.find(std::string(character_name));
	if (it != player_data_map.end()) {
		return it->second;
	}

	return bad_player_data;
};

Player create_player(std::string_view character_name, V2 spawn_pos_ws) {
	Player result = {};

	result.dead = false;
	result.faction = F_Player;
	result.character_name = character_name;

	Player_Data data = get_player_data(character_name);

	// NOTE: AS_Idle is the default starting animation for the player
	result.torso = create_animation_tracker(ATT_Direction_16_Torso, character_name, AS_Walking, APS_Speed_Based, AM_Animate_Looping, true);
	result.legs = create_animation_tracker(ATT_Direction_8_Legs, character_name, AS_Walking, APS_Speed_Based, AM_Animate_Looping, true);

	result.w = data.h;
	result.h = data.w;

	result.rb = create_rigid_body(
		{ spawn_pos_ws.x, spawn_pos_ws.y}, 
		data.base_speed
	);

	result.health_bar = create_health_bar(
		data.hp, 
		Globals::DEFAULT_HEALTH_BAR_WIDTH, 
		Globals::DEFAULT_HEALTH_BAR_HEIGHT, 
		data.h / 2 + Globals::DEFAULT_HEALTH_BAR_HEIGHT
	);

	float collider_radius = (float)result.h / 6.0f;
	add_collider(&result.rb, { 0, (float)result.h / 4.0f  }, collider_radius);
	add_collider(&result.rb, { 0, 		      0 }, collider_radius);
	add_collider(&result.rb, { 0, (float)-result.h / 4.0f }, collider_radius);

	return result;
}

void draw_player(Player& p, V2 camera_ws_pos) {
	// WE ARE DRAWING THE PLAYER RELATIVE TO THE CAMERA.
	MP_Rect p_draw_rect = {
		(int)p.rb.pos_ws.x - p.w / 2,
		(int)p.rb.pos_ws.y - p.h / 2,
		p.w, p.h};
	V2 p_pos_cs = convert_ws_to_cs({ (float)p_draw_rect.x, (float)p_draw_rect.y }, camera_ws_pos);
	p_draw_rect.x = (int)p_pos_cs.x; 
	p_draw_rect.y = (int)p_pos_cs.y; 
	draw_animation_tracker(&p.legs, p_draw_rect, 0);
	draw_animation_tracker(&p.torso, p_draw_rect, 0);

	V2 health_bar_cs_pos = p.rb.pos_ws;
	health_bar_cs_pos.y += p.health_bar.offset;
	health_bar_cs_pos = convert_ws_to_cs(health_bar_cs_pos, camera_ws_pos);
	draw_faction_health_bar(p.faction, p.health_bar, health_bar_cs_pos);

	if (Globals::debug_show_colliders) {
		draw_colliders(&p.rb, camera_ws_pos);
	}
}

void delete_player(Player* player) {
	delete_weapon(player->weapon);
	*player = {};
}

