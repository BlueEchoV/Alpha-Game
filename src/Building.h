#pragma once
#include "Utility.h"
#include "Game_Utility.h"
#include "Entity_Manager.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "Player.h"

#include <vector>

struct Building_Data {
    std::string building_name;
    std::string sprite_sheet_name;
    int damage;
    int hp;
    int w, h;
};

struct Building {
    std::string building_name;
    Rigid_Body rb;  
    Animation_Tracker at;
    Health_Bar health_bar;
    int w, h;
    int upgrade_level = 0;
    bool is_wall = false; 
    bool destroyed = false;
    Handle handle;
    // Add any building-specific fields, e.g., passive_damage for spiked walls
};

void spawn_building(std::string_view building_name, bool is_wall, V2 pos, Storage<Building>& storage, std::vector<Handle>& handles);
void update_building(Building& building, float dt);
void draw_building_outlined(Building& building, V2 camera_pos);
void destroy_building(Building& building);  
void upgrade_building(Building& building);  

void load_buildings_csv(CSV_Data* data);
