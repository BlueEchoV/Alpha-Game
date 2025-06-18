#include "Game_Data.h"

void delete_destroyed_entities_from_game_data_handles(Game_Data& game_data) {
	std::erase_if(game_data.enemy_unit_handles, [&game_data](const Handle& enemy_unit_handle) {
		Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
		if (unit != NULL) {
			if (unit->destroyed) {
				delete_handle(game_data.unit_storage, enemy_unit_handle);
				*unit = {};
				return true;
			}
		}
		return false;
	});
	std::erase_if(game_data.projectile_handles, [&game_data](const Handle& projectile_handles) {
		Projectile* proj = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handles);
		if (proj != NULL) {
			if (proj->destroyed) {
				delete_handle(game_data.unit_storage, projectile_handles);
				*proj = {};
				return true;
			}
		}
		return false;
	});
}

void destroy_game_data(Game_Data& game_data) {
	REF(game_data);
	// delete game_data.player->weapon;
	// delete game_data.player;
	// delete game_data.current_horde;
};
