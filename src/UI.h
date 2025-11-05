#pragma once
#include "Game_Data.h"

extern MP_Rect ready_button_area;

bool is_point_in_rect(MP_Rect* rect, V2 point, bool center);
void draw_game_loop_ui(Game_Data& game_data, Font_Type ft);
