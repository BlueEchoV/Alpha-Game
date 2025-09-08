#include "UI.h"

void draw_game_loop_ui(Game_Data& game_data, Font_Type ft) {
	Font* font = get_font(ft);

	// Draw the spawn region?
	draw_night_wave_spawn_region(CT_Red, game_data.current_night_wave, game_data.world.map, game_data.camera.pos_ws);

	// Draw the total number of waves 
	std::string current_wave = std::to_string(game_data.current_night_wave.current_wave);
	draw_string(*font, current_wave.c_str(), CT_Orange, true, (float)(Globals::playground_area_w / 2), (float)(Globals::playground_area_h / 4), 3, true);
}