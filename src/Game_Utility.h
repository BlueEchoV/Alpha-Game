#pragma once
#include "Renderer.h"

enum Faction {
	F_Allies,
	F_Enemies,
	F_Player
};

struct Cooldown {
	float max;
	float current;
};
Cooldown create_cooldown(float max_cd);
void update_cooldown(Cooldown& cd, float delta_time);
bool cooldown_ready(Cooldown& cd);
bool trigger_cooldown(Cooldown& cd);

struct Health_Bar {
	int offset;
	int max_hp;
	int current_hp;
	int health_bar_w, health_bar_h;
	int attached_entity_w, attached_entity_h;
};

Health_Bar create_health_bar(int hp, int hb_w, int hb_h, int attached_entity_w, int attached_entity_h, int offset);
void draw_outline_box(Color_Type c, MP_Rect* rect, int outline_thickness, bool exclude_outline_from_w_and_h, bool center);
void draw_health_bar(Color_Type c, Health_Bar& health_bar, V2 pos_cs, bool is_pos_cs_centered);
void draw_faction_health_bar(Faction faction, Health_Bar& health_bar, V2 pos_cs, bool is_pos_cs_centered);

