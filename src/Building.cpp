#include "Building.h"
#include "Entity_Manager.h"

Building_Data bad_building_data = {
	"bone_turret", "bone_turret_s", Globals::tile_w, Globals::tile_h * 2, 1, 1, 100, "bone_launcher", 50
};

std::unordered_map<std::string, Building_Data> building_data_map;

Building_Data* get_building_data(std::string building_type) {
	if (building_data_map.find(building_type) != building_data_map.end()) {
		return &building_data_map[building_type];
	}

	return &bad_building_data;
}

void spawn_building(std::string_view building_name, bool is_wall, V2 pos_ws, Storage<Building>& storage, std::vector<Handle>& handles) {
	Building result = {};

	int tile_taken_x = 0;
	int tile_taken_y = 0;
	get_tile_pos_index_from_pos_ws(pos_ws, tile_taken_x, tile_taken_y);

	Building_Data* data = get_building_data(std::string(building_name));

    // Collision footprint: tile counts from bottom-left, as specified
    int collision_tile_offset_x = data->collision_tile_offset_x;
    int collision_tile_offset_y = data->collision_tile_offset_y;

    // Validate that the proposed collision footprint does not overlap with any existing building's collision footprint
    int new_coll_start_x = tile_taken_x;
    int new_coll_start_y = tile_taken_y;
    for (int dx = 1; dx <= collision_tile_offset_x; dx++) {
        for (int dy = 1; dy <= collision_tile_offset_y; dy++) {
            int check_x = new_coll_start_x + dx;
            int check_y = new_coll_start_y + dy;

            for (Handle handle : handles) {
                Building* b = get_entity_pointer_from_handle(storage, handle);
                if (b == nullptr) {
                    continue;
                }

                int b_coll_start_x = b->tile_x;
                int b_coll_start_y = b->tile_y;
                int b_coll_end_x = b_coll_start_x + b->collision_tile_offset_x;
                int b_coll_end_y = b_coll_start_y + b->collision_tile_offset_y;

                // Check if the candidate collision tile falls within the existing building's collision rectangular footprint
                if (check_x >= b_coll_start_x && check_x < b_coll_end_x &&
                    check_y >= b_coll_start_y && check_y < b_coll_end_y) {
                    return; // Overlap detected; abort spawning
                }
            }
        }
    }

    // No overlaps; proceed with spawning
    result.tile_x = tile_taken_x;
    result.tile_y = tile_taken_y;

	// Set collision footprint
    result.collision_tile_offset_x = collision_tile_offset_x;
    result.collision_tile_offset_y = collision_tile_offset_y;

	result.building_name = building_name;
	result.destroyed = false;

	V2 tile_grid_pos_ws = get_tile_pos_ws_from_grid_index(result.tile_x, result.tile_y);

	result.rb = create_rigid_body(tile_grid_pos_ws, 0);

	result.at = create_animation_tracker(ATT_Direction_2, data->sprite_sheet_name, AS_No_Animation, APS_No_Animation, AM_No_Animation, false);

	result.w = data->w;
	result.h = data->h;

    result.hp = data->hp;

	equip_weapon(result.weapon, data->weapon_name);

    result.attack_range = data->attack_range;

	result.health_bar = create_health_bar(
		data->hp, 
		Globals::DEFAULT_HEALTH_BAR_WIDTH, 
		Globals::DEFAULT_HEALTH_BAR_HEIGHT, 
		data->h / 2 + Globals::DEFAULT_HEALTH_BAR_HEIGHT
	);

	result.upgrade_level = 0;
	result.is_wall = is_wall;
	result.destroyed = false;

	result.handle = create_handle(storage);
	handles.push_back(result.handle);

	storage.storage[result.handle.index] = result;
}

void update_building(Building& building, float dt, std::vector<Handle>& enemy_handles, Storage<Unit>& unit_storage,
	std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage, Camera camera) {
	// Attack Speed, Damage, Attack Range

	if (building.weapon->can_fire) {
		Unit* target = nullptr;
		int min_distance = building.attack_range;  // Track closest for priority

		for (Handle handle : enemy_handles) {
			Unit* enemy = get_entity_pointer_from_handle(unit_storage, handle);
			if (enemy->dead) continue;  // Skip dead enemies

			for (int i = 0; i < enemy->rb.num_colliders; i++) {  // Assuming Unit has Rigid_Body rb
				Collider* collider_a = &enemy->rb.colliders[i];
				V2 collider_a_ws_pos = enemy->rb.pos_ws + collider_a->pos_ls;
				V2 collider_b_ws_pos = building.rb.pos_ws;

				int distance = (int)calculate_distance_between(collider_a_ws_pos, collider_b_ws_pos);
				if (distance < (collider_a->radius + building.attack_range) && distance < min_distance) {
					min_distance = distance;
					target = enemy;
				}
			}
		}

		if (target != nullptr) {
			building.weapon->fire_weapon(projectile_handles, projectile_storage, camera, building.rb.pos_ws,target->rb.pos_ws, F_Player);
		}
	}

	building.weapon->update_weapon(dt);
}

void draw_building_outlined(Building& building, V2 camera_pos) {
	V2 cs_pos = convert_ws_to_cs(building.rb.pos_ws, camera_pos);

	MP_Rect dst_rect = {(int)cs_pos.x, (int)cs_pos.y, building.w, building.h};

	draw_animation_tracker_outlined(&building.at, dst_rect, 0, CT_Black, 1);
}

// void destroy_building(Building& building);  

void upgrade_building(Building& building) {
	REF(building);
}

Type_Descriptor building_data_type_descriptors[] = {
	FIELD(Building_Data, VT_String, building_name),
	FIELD(Building_Data, VT_String, sprite_sheet_name),
	FIELD(Building_Data, VT_Int, w),
	FIELD(Building_Data, VT_Int, h),
	FIELD(Building_Data, VT_Int, collision_tile_offset_x),
	FIELD(Building_Data, VT_Int, collision_tile_offset_y),
	FIELD(Building_Data, VT_Int, hp),
	FIELD(Building_Data, VT_String, weapon_name),
	FIELD(Building_Data, VT_Int, attack_range)
};

void load_building_data_csv(CSV_Data* data) {
	std::vector<Building_Data> building_data;
	building_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_building_data_type_descriptors(building_data_type_descriptors);

	load_csv_data_file(data, (char*)building_data.data(), safe_building_data_type_descriptors, sizeof(Building_Data));

	for (Building_Data& building_data_iterator : building_data) {
		building_data_map[building_data_iterator.building_name] = building_data_iterator;
	}
}
