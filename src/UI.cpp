#include "UI.h"

void draw_game_loop_ui(Game_Data& game_data, Font_Type ft) {
	Font* font = get_font(ft);

	MP_Rect ready_button_area = { (int)((float)Globals::playground_area_w * 0.5f), (int)((float)Globals::playground_area_h * 0.90f), 160, 40};
	Night_Wave* nw = &game_data.current_night_wave;
	if (nw->begin_spawning == false && nw->current_wave < get_max_waves_from_difficulty(nw->difficulty) && game_data.active_enemy_units <= 0) {
		if (button_text(ready_button_area, "Ready", 3, FT_Basic, CT_Dark_Yellow, true, CT_Black, true)) {
			game_data.current_night_wave.begin_spawning = true;
		}
	}
	std::string current_wave_string = "Current Wave = " + std::to_string(nw->current_wave);
	draw_string(*font, current_wave_string.c_str(), CT_Red_Wine, true, 
		(int)((float)Globals::playground_area_w * 0.5f), (int)((float)Globals::playground_area_h * 0.95f), 3, true);
}