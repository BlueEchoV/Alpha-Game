# include "Building.h"

Building_Data error_building_data = {
	"bone_turret", "bone_turret_s", 100, 10, Globals::tile_w, Globals::tile_h * 2
};

void spawn_building(std::string_view building_name, bool is_wall, V2 pos_ws, Storage<Building>& storage, std::vector<Handle>& handles) {
	Building result = {};

	Building_Data* data = &error_building_data;

	result.building_name = building_name;
	result.destroyed = false;

	V2 tile_grid_pos_ws = get_tile_pos_ws_from_pos_ws(pos_ws);

	result.rb = create_rigid_body(tile_grid_pos_ws, 0);

	result.at = create_animation_tracker(ATT_Direction_2, data->sprite_sheet_name, AS_No_Animation, APS_No_Animation, AM_No_Animation, false);

	result.w = data->w;
	result.h = data->h;

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

void update_building(Building& building, float dt) {
	REF(building);
	REF(dt);
}

void draw_building_outlined(Building& building, V2 camera_pos) {
	V2 cs_pos = convert_ws_to_cs(building.rb.pos_ws, camera_pos);

	MP_Rect dst_rect = {(int)cs_pos.x, (int)cs_pos.y, building.w, building.h};

	draw_animation_tracker_outlined(&building.at, dst_rect, 0, CT_Black, 1);
}

void destroy_building(Building& building);  

void upgrade_building(Building& building) {
	REF(building);
}
