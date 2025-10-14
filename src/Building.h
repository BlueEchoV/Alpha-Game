#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "Player.h"
#include "Unit.h"

#include <vector>

struct Building_Data {
    std::string building_name;
    std::string sprite_sheet_name;
    int w, h;
    int hp;
    std::string weapon_name;
    int attack_range;
};

struct Building {
    std::string building_name;
    Rigid_Body rb;  
    // The tile the building is on
    int tile_x, tile_y;
    Animation_Tracker at;
    Health_Bar health_bar;
    int w, h;

    int hp;

    Weapon* weapon;
    int attack_range;

    int upgrade_level = 0;
    bool is_wall = false; 
    bool destroyed = false;
    Handle handle;
    // Add any building-specific fields, e.g., passive_damage for spiked walls
};

void spawn_building(std::string_view building_name, bool is_wall, V2 pos_ws, Storage<Building>& storage, std::vector<Handle>& handles);
void update_building(Building& building, float dt, std::vector<Handle>& enemy_handles, Storage<Unit>& unit_storage,
    std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage, Camera camera);
void draw_building_outlined(Building& building, V2 camera_pos);
void destroy_building(Building& building);  
void upgrade_building(Building& building);  

void load_building_data_csv(CSV_Data* data);
