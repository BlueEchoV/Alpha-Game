#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"

#include "Ability.h"
#include "Unit.h"
#include "Horde.h"

struct Game_Data {
	Camera camera;

	Player player;

	Storage<Projectile> projectile_storage = { .storage_type = ST_Projectile };
	Storage<Unit>		unit_storage = { .storage_type = ST_Unit };

	std::vector<Handle> enemy_unit_handles;
	std::vector<Handle> projectile_handles;

	Horde current_horde;

	Font_Type selected_font;
};

void delete_destroyed_entities_from_game_data_handles(Game_Data& game_data);
void render(Game_Data& game_data, float delta_time);

void destroy_game_data(Game_Data& game_data);

// 1: I need to load the CSV file
// 2: I need to 
