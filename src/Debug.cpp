#include "Debug.h"

int debug_point_size = 6;
Font_Type debug_font = FT_Basic;
// The draw_at variable should be in world space. World space is technically UI space.
void debug_draw_coor(Game_Data& game_data, V2 coor_to_draw, bool convert_coor_to_draw_to_cs,
	V2 draw_at, bool convert_draw_at_to_cs, Color_Type c, bool background, std::string_view custom_text) {

	V2 coor_to_draw_final = coor_to_draw;
	if (convert_coor_to_draw_to_cs) {
		coor_to_draw_final = convert_ws_to_cs(coor_to_draw_final, game_data.camera.pos_ws);
	}
	V2 draw_at_final = draw_at;
	if (convert_draw_at_to_cs) {
		draw_at_final = convert_ws_to_cs(draw_at, game_data.camera.pos_ws);
	}

	Font* font = get_font(debug_font); 
	int y_offset = font->char_height + font->char_height / 2;

	std::string str = std::string(custom_text) + "x = " + std::to_string((int)coor_to_draw_final.x) + ", y = " 
		+ std::to_string((int)coor_to_draw_final.y);

	draw_quick_string(c, background, str.c_str(), (int)draw_at_final.x, (int)draw_at_final.y + y_offset);
	MP_Rect rect = {}; 
	rect.w = debug_point_size;
	rect.h = debug_point_size;
	rect.x = (int)draw_at_final.x - rect.w / 2;;
	rect.y = (int)draw_at_final.y - rect.h / 2;;
	mp_set_render_draw_color(c);
	mp_render_fill_rect(&rect);
}

void debug_draw_collider_coodinates(Game_Data& game_data, Rigid_Body& rb) {
	for (int i = 0; i < rb.num_colliders; i++) {
		Collider* c = &rb.colliders[i];
		V2 collider_ws_pos = rb.pos_ws + c->pos_ls;
		V2 collider_cs_pos = convert_ws_to_cs(collider_ws_pos, game_data.camera.pos_ws);
		debug_draw_coor( game_data, c->pos_ls, false, collider_cs_pos, false, CT_Dark_Yellow, true, "LS: " );
	}
}

int window_w = 0;
int window_h = 0;
void draw_debug_2d_rotation_matrix_rect(V2 center, Font* font, float delta_time) {
	MP_Renderer* renderer = Globals::renderer;
	
	V2 c = {};
	c.x = center.x;
	c.y = center.y;

	float w = 100;
	float h = 100;

	bool recalculate_points = false;
	if (window_w != renderer->window_width || 
		window_h != renderer->window_height) {
		recalculate_points = true;
	}
	window_w = renderer->window_width;
	window_h = renderer->window_height;

	V2 original_top_left = { c.x - (w / 2.0f), (int)c.y + (h / 2) };
	V2 original_top_right = { c.x + (w / 2.0f), (int)c.y + (h / 2) };
	V2 original_bottom_right = { c.x + (w / 2.0f), (int)c.y - (h / 2) };
	V2 original_bottom_left = { c.x - (w / 2.0f), (int)c.y - (h / 2) };

	static V2 new_top_left = original_top_left;
	static V2 new_top_right = original_top_right;
	static V2 new_bottom_right = original_bottom_right;
	static V2 new_bottom_left = original_bottom_left;

	static float angle = 0.0f;
	static float last_angle = 0.0f;
	float rotation_speed = 8.0f;
	angle += delta_time * rotation_speed;

	if (angle != last_angle || recalculate_points) {
		new_top_left = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_top_left.x, (float)original_top_left.y);
		new_top_right = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_top_right.x, (float)original_top_right.y);
		new_bottom_right = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_bottom_right.x, (float)original_bottom_right.y);
		new_bottom_left = rotate_point_based_off_angle(angle, c.x, c.y, (float)original_bottom_left.x, (float)original_bottom_left.y);
		last_angle = angle;
	}
	int string_size = 1;
	std::string center_string = {};
	center_string = "(" + std::to_string((int)c.x) + " " + std::to_string((int)c.y) + ")";
	draw_string(*font, center_string.c_str(), CT_White, true, c.x, c.y, string_size, true);

	std::string angle_string = {};
	center_string = std::to_string((int)angle);
	draw_string(*font, center_string.c_str(), CT_White, true, (int)c.x, (int)c.y + (font->char_height * 2) * string_size, string_size, true);

	std::string top_left_string = {};
	top_left_string = "(" + std::to_string((int)new_top_left.x) + " " + std::to_string((int)new_top_left.y) + ")";
	draw_string(*font, top_left_string.c_str(), CT_White, true, new_top_left.x, new_top_left.y, string_size, true);

	std::string top_right_string = {};
	top_right_string = "(" + std::to_string((int)new_top_right.x) + " " + std::to_string((int)new_top_right.y) + ")";
	draw_string(*font, top_right_string.c_str(), CT_White, true, new_top_right.x, new_top_right.y, string_size, true);

	std::string bottom_right_string = {};
	bottom_right_string = "(" + std::to_string((int)new_bottom_right.x) + " " + std::to_string((int)new_bottom_right.y) + ")";
	draw_string(*font, bottom_right_string.c_str(), CT_White, true, new_bottom_right.x, new_bottom_right.y, string_size, true);

	std::string bottom_left_string = {};
	bottom_left_string = "(" + std::to_string((int)new_bottom_left.x) + " " + std::to_string((int)new_bottom_left.y) + ")";
	draw_string(*font, bottom_left_string.c_str(), CT_White, true, new_bottom_left.x, new_bottom_left.y, string_size, true);

	mp_set_render_draw_color(CT_Orange);
	mp_render_draw_line((int)new_top_left.x, (int)new_top_left.y, (int)new_top_right.x, (int)new_top_right.y);
	mp_render_draw_line((int)new_top_right.x, (int)new_top_right.y, (int)new_bottom_right.x, (int)new_bottom_right.y);
	mp_render_draw_line((int)new_bottom_right.x, (int)new_bottom_right.y, (int)new_bottom_left.x, (int)new_bottom_left.y);
	mp_render_draw_line((int)new_bottom_left.x, (int)new_bottom_left.y, (int)new_top_left.x, (int)new_top_left.y);
}

void debug_draw_mp_renderer_visualizations(Font& font, MP_Texture* debug_texture, float delta_time) {
	MP_Renderer* renderer = Globals::renderer;

	int width = 150;
	int height = 150;

	int starting_x = 50;
	int starting_y = renderer->window_height - (50 + height);

	int offset_x = 20 + width;
	int offset_y = -50 - height;

	int images_per_row = 5;
	int string_size = 2;

	MP_Rect background_rects[DI_Count] = {};
	int background_padding = 6;

	mp_set_render_draw_color(0, 0, 0, 255);
	for (int i = 0; i < DI_Count; i++) {
		int x = (starting_x + (i % images_per_row) * offset_x);
		int y = starting_y + (i / images_per_row) * offset_y;

		background_rects[i].x = x - background_padding;
		background_rects[i].y = y - background_padding;
		background_rects[i].w = width + background_padding * 2;
		background_rects[i].h = (height + background_padding * 2) + (font.char_height * (string_size + 1)) + 5;
	}
	mp_render_fill_rects(background_rects, DI_Count);

	// First rows
	for (int i = 0; i < DI_Count; i++) {
		int x = (starting_x + (i % images_per_row) * offset_x);
		int y = starting_y + (i / images_per_row) * offset_y;
		int title_x = x + width / 2;
		int title_y = y + (font.char_height * (string_size - 1)) + (height) + (font.char_height * 2) / 2;

		MP_Rect rect = { x, y, width, height };

		switch ((Debug_Image)i) {
		case DI_mp_render_fill_rect: {
			draw_string(font, "fill_rect", CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Red);
			mp_render_fill_rect(&rect);
			break;
		}
		case DI_mp_render_fill_rects: {
			draw_string(font, "fill_rects", CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Green);
			MP_Rect bottom_left_and_top_right[2] = {
				{x		      , y             , width / 2, height / 2},
				{x + width / 2, y + height / 2, width / 2, height / 2}
			};
			mp_render_fill_rects(bottom_left_and_top_right, ARRAYSIZE(bottom_left_and_top_right));

			mp_set_render_draw_color(CT_Blue);
			MP_Rect bottom_right_and_top_left[2] = {
				{x + width / 2, y			  , width / 2, height / 2},
				{x            , y + height / 2, width / 2, height / 2}
			};
			mp_render_fill_rects(bottom_right_and_top_left, ARRAYSIZE(bottom_right_and_top_left));
			break;
		}
		case DI_mp_render_draw_line: {
			draw_string(font, "draw_line", CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Red);
			mp_render_draw_line(x, y, x + width, y);

			mp_set_render_draw_color(CT_Green);
			mp_render_draw_line(x, y + height / 2, x + width, y + height / 2);

			mp_set_render_draw_color(CT_Blue);
			mp_render_draw_line(x, y + height, x + width, y + height);

			mp_set_render_draw_color(CT_Red);
			mp_render_draw_line(x, y, x, y + height);

			mp_set_render_draw_color(CT_Green);
			mp_render_draw_line(x + width / 2, y, x + width / 2, y + height);

			mp_set_render_draw_color(CT_Blue);
			mp_render_draw_line(x + width, y, x + width, y + height);
			break;
		}
		case DI_mp_render_draw_lines: {
			draw_string(font, "draw_lines",CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Dark_Yellow);

			const int total_points = 5;
			MP_Point points[total_points];

			points[0].x = x;
			points[0].y = y;

			points[1].x = x;
			points[1].y = y + height;

			points[2].x = x + width;
			points[2].y = y + height;

			points[3].x = x + width;
			points[3].y = y;

			points[4].x = x;
			points[4].y = y;

			mp_render_draw_lines(points, total_points);
			break;
		}
		case DI_mp_render_draw_point: {
			draw_string(font, "draw_point", CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Red);
			mp_render_draw_point(x, y);
			mp_render_draw_point(x + width, y);
			mp_render_draw_point(x, y + height);
			mp_render_draw_point(x + width, y + height);

			mp_render_draw_point(x + width / 4, y + height / 4);
			mp_render_draw_point(x + ((width / 4) * 3), y + height / 4);
			mp_render_draw_point(x + ((width / 4) * 3), y + ((height / 4) * 3));
			mp_render_draw_point(x + width / 4, y + ((height / 4) * 3));
			break;
		}
		case DI_mp_render_draw_points: {
			draw_string(font, "draw_points", CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Green);
			const int grid_size = 4;
			const int total_points = grid_size * grid_size;
			MP_Point points[total_points] = {};

			int x_spacing = (int)((float)width / ((float)grid_size - 1.0f));
			int y_spacing = (int)((float)height / ((float)grid_size - 1.0f));

			// Populate the points array
			int index = 0;
			for (int z = 0; z < grid_size; ++z) {
				for (int j = 0; j < grid_size; ++j) {
					points[index++] = { x + j * x_spacing, y + z * y_spacing };
				}
			}

			mp_render_draw_points(points, total_points);
			break;
		}
		case DI_mp_render_draw_rect: {
			draw_string(font, "draw_rect", CT_White, true, title_x, title_y, string_size, true);
			mp_set_render_draw_color(CT_Red);

			MP_Rect draw_rect = {};
			draw_rect.x = x;
			draw_rect.y = y;
			draw_rect.w = width;
			draw_rect.h = height;

			mp_render_draw_rect(&draw_rect);
			break;
		}
		case DI_mp_render_draw_rects: {
			draw_string(font, "draw_rect", CT_White, true, title_x, title_y, string_size, true);

			mp_set_render_draw_color(CT_Dark_Yellow);

			const int total_rects = 4;
			MP_Rect draw_rects[total_rects] = {};

			draw_rects[0] = { x + width / 2, y			  , width / 2, height / 2 };
			draw_rects[1] = { x			  , y + height / 2, width / 2, height / 2 };
			draw_rects[2] = { x			  , y			  , width / 2, height / 2 };
			draw_rects[3] = { x + width / 2, y + height / 2, width / 2, height / 2 };

			mp_render_draw_rects(draw_rects, total_rects);
			break;
		}
		case DI_mp_render_copy: {
			draw_string(font, "render_copy", CT_White, true, title_x, title_y, string_size, true);

			MP_Rect dst_rect = { x, y, width, height };
			mp_set_texture_alpha_mod(debug_texture, 255);
			mp_render_copy(debug_texture, NULL, &dst_rect);
			break;
		}
		case DI_mp_render_copy_alpha: {
			draw_string(font, "alpha_mod", CT_White, true, title_x, title_y, string_size, true);

			MP_Rect dst_rect = { x, y, width, height };
			mp_set_texture_alpha_mod(debug_texture, 100);
			mp_render_copy(debug_texture, NULL, &dst_rect);
			break;
		}
		case DI_2d_matrix_transformation_rect: {
			draw_string(font, "2d_rotation", CT_White, true, title_x, title_y, string_size, true);
			draw_debug_2d_rotation_matrix_rect({ (float)x + width / 2, (float)y + height / 2 }, &font, delta_time);
			break;
		}
		case DI_copy_ex: {
			draw_string(font, "rnd_cpy_ex", CT_White, true, title_x, title_y, string_size, true);

			MP_Rect temp_rect = { x, y, width, height };
			static float temp_angle = 0;
			static float rotation_speed = 8.0f;
			temp_angle += delta_time * rotation_speed;
			mp_render_copy_ex(images["IT_Sun"].texture, NULL, &temp_rect, temp_angle, NULL, SDL_FLIP_NONE);
			break;
		}
		case DI_button_text: {
			draw_string(font, "button", CT_White, true, title_x, title_y, string_size, true);
			MP_Rect button_area = { x, y, width, height };
			if (button_text(button_area, "Click Me", 2, FT_Basic, CT_White, true, CT_Black, false)) {
				log("Button Pressed!!!");
			}
			break;
		}
		case DI_button_image: {
			draw_string(font, "button", CT_White, true, title_x, title_y, string_size, true);
			MP_Rect button_area = { x, y, width, height };
			if (button_image(button_area, "tree", NULL, false)) {
				log("Button Pressed!!!");
			}
			break;
		}
		default: {
			draw_string(font, "Error. Image default case hit in draw debug image function", CT_White, true, x, y, 1, true);
			break;
		}
		}
	}
}

void debug_draw_coordinates(Game_Data& game_data) {
	MP_Renderer* renderer = Globals::renderer;
	V2 mouse = get_viewport_mouse_position(renderer->open_gl.window_handle);
	// The mouse position is already in camera space
	debug_draw_coor(game_data, mouse, false, mouse, false, CT_Green, true, "Mouse: ");
	debug_draw_coor(game_data, game_data.player.rb.pos_ws, false, 
		{(float)Globals::playground_area_w / 2, (float)Globals::playground_area_h / 2}, false,
		CT_Green, true, "Player WS Pos: ");
}

void debug_draw_stats(Font& font, MP_Texture* debug_texture) {
	int stats_string_size = 2;
	int stats_border_padding = 50;
	int stats_w = 300;
	int stats_h = 600;
	int stats_rect_x = Globals::playground_area_w - (stats_w + stats_border_padding);
	int stats_rect_y = Globals::playground_area_h - (stats_h + stats_border_padding);
	MP_Rect stats_rect = {stats_rect_x, stats_rect_y, stats_w, stats_h};
	mp_set_render_draw_color(CT_Black);
	mp_render_fill_rect(&stats_rect);

	int stats_string_x = stats_rect_x + stats_w / 2;
	int stats_string_y = (stats_rect_y + stats_h) - (font.char_height * stats_string_size);
	int stats_string_y_offset = font.char_height * stats_string_size * 2;

	std::string stats_title = "Debug Stats";
	draw_string(font, stats_title.c_str(), CT_Red, true, stats_string_x, stats_string_y, 
		stats_string_size, true);

	stats_string_y -= stats_string_y_offset;

	std::string total_arrows_fired = "Arrows Fired: " + std::to_string(Globals::debug_total_arrows);
	draw_string(font, total_arrows_fired.c_str(), CT_Red, true, (int)stats_string_x, (int)stats_string_y, 
		stats_string_size, true);

	mp_set_texture_color_mod(font.image.texture, 255, 255, 255);
	mp_set_texture_alpha_mod(debug_texture, 255);
}

void debug_draw_animation_trackers(Font& font, float delta_time) {
	const int animation_play_speed = 300;

	static std::vector<Animation_Tracker> trackers = {
		create_animation_tracker(ATT_Direction_8, "hellhound", AS_Walking, APS_Speed_Based, AM_Animate_Looping, true),
		create_animation_tracker(ATT_Direction_8, "gravebound_peasant", AS_Walking, APS_Speed_Based, AM_Animate_Looping, true),
		create_animation_tracker(ATT_Direction_8_Atlas, "ravenous_skulk", AS_Walking, APS_Speed_Based, AM_Animate_Looping, true)
	};

	static bool first_pass = true;
	if (first_pass) {
		for (Animation_Tracker& at : trackers) {
				first_pass = false;
				V2 pointing_south = {0.0f, -1.0f};
				change_animation_tracker(&at, at.entity_name, AS_Walking, APS_Speed_Based, AM_Animate_Looping, false, pointing_south);
		}
	}

	for (Animation_Tracker& at : trackers) {
		update_animation_tracker(&at, delta_time, animation_play_speed);
	}

	MP_Renderer* renderer = Globals::renderer;

	int width = 150;
	int height = 150;

	int starting_x = 50;
	int starting_y = renderer->window_height - (50 + height);

	int offset_x = 20 + width;
	int offset_y = -50 - height;

	int images_per_row = 5;
	int string_size = 1;

	int num = (int)trackers.size();

	std::vector<MP_Rect> background_rects(num);
	int background_padding = 6;

	mp_set_render_draw_color(0, 0, 0, 255);
	for (int i = 0; i < num; ++i) {

		int x = starting_x + (i % images_per_row) * offset_x;
		int y = starting_y + (i / images_per_row) * offset_y;

		background_rects[i].x = x - background_padding;
		background_rects[i].y = y - background_padding;
		background_rects[i].w = width  + background_padding * 2;
		background_rects[i].h = height + background_padding * 2
							  + font.char_height * (string_size + 1) + 5;
	}

	mp_render_fill_rects(background_rects.data(),
						 static_cast<int>(background_rects.size()));
	// First rows
	for (int i = 0; i < trackers.size(); i++) {
		int x = (starting_x + (i % images_per_row) * offset_x);
		int y = starting_y + (i / images_per_row) * offset_y;
		int title_x = x + width / 2;
		int title_y = y + (font.char_height * (string_size - 1)) + (height) + (font.char_height * 2) / 2;

		MP_Rect rect = { x, y, width, height };

		draw_string(font, trackers[i].entity_name.c_str(), CT_White, true, title_x, title_y, string_size, true);
		draw_animation_tracker(&trackers[i], rect, 0);
	}
}

void debug_draw_all_debug_info(Game_Data& game_data, Font& font, MP_Texture* debug_texture, float delta_time) {
	if (Globals::toggle_debug_images) {
		debug_draw_mp_renderer_visualizations(font, debug_texture, delta_time);
	}
	if (Globals::debug_show_coordinates) {
		debug_draw_coordinates(game_data);
		V2 mouse_cs_pos = get_viewport_mouse_position(Globals::renderer->open_gl.window_handle);
		V2 player_cs_pos = convert_ws_to_cs(game_data.player.rb.pos_ws, game_data.camera.pos_ws);
		mp_render_draw_line(player_cs_pos.x, player_cs_pos.y, mouse_cs_pos.x, mouse_cs_pos.y);
	}
	if (Globals::debug_show_stats) {
		debug_draw_stats(font, debug_texture);
	}
	if (Globals::toggle_debug_unit_images) {
		debug_draw_animation_trackers(font, delta_time);
	}
}

