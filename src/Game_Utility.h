#pragma once
#include "Renderer.h"

struct Camera {
	V2 pos_ws;
	int w, h;
};

Camera create_camera(V2 player_pos_ws);
void update_camera(Camera& camera, V2 player_pos);

enum Faction {
	F_Player,
	F_Allies,
	F_Enemies
};

struct Cooldown {
	float max;
	float current;
};
Cooldown create_cooldown(float max_cd);
bool check_and_update_cooldown(Cooldown& cd, float delta_time);

struct Health_Bar {
	int offset;
	int max_hp;
	int current_hp;
	int w, h;
};

Health_Bar create_health_bar(int hp, int w, int h, int offset);
void draw_health_bar(Color_Type c, Health_Bar& health_bar, V2 pos);
void draw_faction_health_bar(Faction faction, Health_Bar& health_bar, V2 pos);

