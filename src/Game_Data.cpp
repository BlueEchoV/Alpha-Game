#include "Game_Data.h"

void prepare_draw_order(Game_Data& game_data) {
    // Update y positions for all Draw_Order entries
    for (Handle& draw_order_handle : game_data.entities_draw_order_handles) {
        Draw_Order* draw_order = get_entity_pointer_from_handle(game_data.entities_draw_order_storage, draw_order_handle);
        if (draw_order != nullptr) {
            switch (draw_order->et) {
            case ET_Player:
                draw_order->y = game_data.player.rb.pos_ws.y;
                break;
            case ET_Enemy_Unit: {
                Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, draw_order->h);
                if (unit != nullptr) {
                    draw_order->y = unit->rb.pos_ws.y;

					if (Globals::debug_show_colliders && unit->dead == false) {
						draw_colliders(&unit->rb, game_data.camera.pos_ws);
					}
                }
                break;
            }
            case ET_Building: {
                Building* building = get_entity_pointer_from_handle(game_data.building_storage, draw_order->h);
                if (building != nullptr) {
                    draw_order->y = building->rb.pos_ws.y;
                }
                break;
            }
            case ET_Projectile: {
                Projectile* projectile = get_entity_pointer_from_handle(game_data.projectile_storage, draw_order->h);
                if (projectile != nullptr) {
                    draw_order->y = projectile->rb.pos_ws.y;
				    // if (Globals::debug_show_coordinates) {
					// 	debug_draw_coor(game_data, p->rb.pos_ws, false, p->rb.pos_ws, true, CT_Green, true, "WS Pos: ");
					// }
					if (Globals::debug_show_colliders) {
						draw_colliders(&projectile->rb, game_data.camera.pos_ws);
					}
                }
                break;
            }
            }
        }
    }

    // Sort by updated y-coordinate
    std::sort(game_data.entities_draw_order_handles.begin(), game_data.entities_draw_order_handles.end(), [&](Handle& a, Handle& b) {
        Draw_Order* draw_order_1 = get_entity_pointer_from_handle(game_data.entities_draw_order_storage, a);
        Draw_Order* draw_order_2 = get_entity_pointer_from_handle(game_data.entities_draw_order_storage, b);
        if (draw_order_1 == nullptr || draw_order_2 == nullptr) {
            return false;
        }
        return draw_order_1->y > draw_order_2->y;
    });
}

void render_game(Game_Data& game_data) {
    prepare_draw_order(game_data);

    for (const Handle& h : game_data.entities_draw_order_handles) {
        Draw_Order* draw_order = get_entity_pointer_from_handle(game_data.entities_draw_order_storage, h);
        if (draw_order == NULL) {
            continue;
        }

        switch (draw_order->et) {
        case ET_Player: {
            render_player(game_data.player, game_data.camera.pos_ws);
            break;
        }
        case ET_Enemy_Unit: {
            Unit* u = get_entity_pointer_from_handle(game_data.unit_storage, draw_order->entity_handle);

            if (u != NULL) {
                render_unit_outlined(*u, game_data.camera.pos_ws, CT_Black, 0.5f);
            }
            break;
        }
        case ET_Building: {
            Building* b = get_entity_pointer_from_handle(game_data.building_storage, draw_order->entity_handle);
            if (b != NULL) {
                render_building_outlined(*b, game_data.camera.pos_ws);
            }
            break;
        }
        case ET_Projectile: {
            Projectile* p = get_entity_pointer_from_handle(game_data.projectile_storage, draw_order->entity_handle);
            if (p != NULL) {
                render_projectile(*p, game_data.camera.pos_ws);
            }
            break;
        }
        }
    }
}

void delete_destroyed_entities_from_game_data_handles(Game_Data& game_data) {
    // Handle enemy units
    std::erase_if(game_data.enemy_unit_handles, [&game_data](const Handle& enemy_unit_handle) {
        Unit* unit = get_entity_pointer_from_handle(game_data.unit_storage, enemy_unit_handle);
        if (unit != nullptr) {
            if (unit->destroyed) {
                delete_handle(game_data.unit_storage, enemy_unit_handle);
                if (unit->draw_order_handle.generation != 0) { // Check for valid handle
                    delete_handle(game_data.entities_draw_order_storage, unit->draw_order_handle);
                }
                *unit = {}; // Optional
                return true;
            }
        }
        return false;
    });

    // Handle projectiles
    std::erase_if(game_data.projectile_handles, [&game_data](const Handle& projectile_handle) {
        Projectile* proj = get_entity_pointer_from_handle(game_data.projectile_storage, projectile_handle);
        if (proj != nullptr) {
            if (proj->destroyed) {
                delete_handle(game_data.projectile_storage, projectile_handle);
                if (proj->draw_order_handle.generation != 0) {
                    delete_handle(game_data.entities_draw_order_storage, proj->draw_order_handle);
                }
                *proj = {}; // Optional
                return true;
            }
        }
        return false;
    });

    // Handle buildings
    std::erase_if(game_data.building_handles, [&game_data](const Handle& building_handle) {
        Building* building = get_entity_pointer_from_handle(game_data.building_storage, building_handle);
        if (building != nullptr) {
            if (building->destroyed) {
                delete_handle(game_data.building_storage, building_handle);
                if (building->draw_order_handle.generation != 0) {
                    delete_handle(game_data.entities_draw_order_storage, building->draw_order_handle);
                }
                *building = {}; // Optional
                return true;
            }
        }
        return false;
    });

    // Handle player (if destroyed)
    if (game_data.player.dead) {
        delete_player(&game_data.player, game_data.entities_draw_order_storage);
    }
}

void destroy_game_data(Game_Data& game_data) {
	REF(game_data);
	// delete game_data.player->weapon;
	// delete game_data.player;
	// delete game_data.current_night_wave;
	// delete player abilities
};
