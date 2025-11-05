#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Menu.h"
#include "Ability.h"
#include "Unit.h"
#include "Night_Wave.h"
#include "Building.h"

#include <algorithm>

struct Game_Data {
	Camera camera;

	Player player;

	Storage<Projectile> projectile_storage;
	Storage<Unit>		unit_storage;
	Storage<Building>   building_storage;
	Storage<Draw_Order> entities_draw_order_storage;

	std::vector<Handle> enemy_unit_handles;
	int active_enemy_units;
	std::vector<Handle> projectile_handles;
	std::vector<Handle> building_handles;
	std::vector<Handle> entities_draw_order_handles;

	// Emipheral - Handles not needed
	std::vector<Damage_Number> damage_numbers;

	Night_Wave current_night_wave;
	World world;

	Font_Type selected_font;

    Game_Data() : projectile_storage(ST_Projectile),
          unit_storage(ST_Unit),
          building_storage(ST_Building),
          entities_draw_order_storage(ST_Draw_Order) {
    }
};

void delete_destroyed_entities_from_game_data_handles(Game_Data& game_data);
void render_game(Game_Data& game_data);
// void render(Game_Data& game_data, float delta_time);

void destroy_game_data(Game_Data& game_data);
