#include "Game.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

// Camera's position is relative to the player
void update_camera(Camera& camera, Player& player) {
	camera.pos_ws.x = (player.pos.x - (Globals::resolution_x / 2.0f));
	camera.pos_ws.y = (player.pos.y - (Globals::resolution_y / 2.0f));

	camera.pos_ws.x += player.w / 2;
	camera.pos_ws.y += player.h / 2;

	camera.w = Globals::resolution_x;
	camera.h = Globals::resolution_y;
}

Camera create_camera(Player& player) {
	Camera result = {};

	update_camera(result, player);

	return result;
}

void draw_circle(Color_Type c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines) {
	V2 pos_cs = convert_ws_to_cs(center_pos_ws, camera_pos);

	mp_set_render_draw_color(c);

	float increment_angle = 360.0f / total_lines;
	float current_degrees = 0;
	for (int i = 0; i < total_lines; i++) {
		float current_radians = convert_degrees_to_radians(current_degrees);
		float force_x = cos(current_radians);
		float force_y = sin(current_radians);
		float x1 = force_x * radius;
		float y1 = force_y * radius;
		current_degrees += increment_angle;

		current_radians = convert_degrees_to_radians(current_degrees);
		force_x = cos(current_radians);
		force_y = sin(current_radians);
		float x2 = force_x * radius;
		float y2 = force_y * radius;

		x1 += pos_cs.x + radius / 2;
		y1 += pos_cs.y + radius / 2;
		x2 += pos_cs.x + radius / 2;
		y2 += pos_cs.y + radius / 2;

		mp_render_draw_line((int)x1, (int)y1, (int)x2, (int)y2);
	}
}

int debug_point_size = 6;
Font_Type debug_font = FT_Basic;
void debug_draw_coor_cs(Color_Type c, bool background, V2 camera_pos, int x, int y, bool coordinates_are_in_ws) {
	Font* font = get_font(debug_font); 
	int y_offset = font->char_height + font->char_height / 2;

	V2 pos_cs = { (float)x, (float)y };
	if (coordinates_are_in_ws) {
		pos_cs = convert_ws_to_cs({ (float)x, (float)y }, camera_pos);
	}
	std::string coordinates = "CS: x = " + std::to_string(x) + ", y = " + std::to_string(y);
	draw_quick_string(c, background, coordinates.c_str(), (int)pos_cs.x, (int)pos_cs.y + y_offset);
	MP_Rect rect = {}; 
	rect.w = debug_point_size;
	rect.h = debug_point_size;
	rect.x = (int)pos_cs.x - rect.w / 2;;
	rect.y = (int)pos_cs.y - rect.h / 2;;
	mp_set_render_draw_color(c);
	mp_render_fill_rect(&rect);
}

void debug_draw_coor_ws(Color_Type c, bool background, V2 camera_pos, int x, int y, bool coordinates_are_in_cs) {
	Font* font = get_font(debug_font); 
	int y_offset = font->char_height;

	V2 pos_ws = { (float)x, (float)y };
	if (coordinates_are_in_cs) {
		// The coordinates are in camera space
		pos_ws = convert_cs_to_ws({ (float)x, (float)y }, camera_pos);
	}
	std::string coordinates = "WS: x = " + std::to_string(x) + ", y = " + std::to_string(y);
	draw_quick_string(c, background, coordinates.c_str(), (int)pos_ws.x, (int)pos_ws.y + y_offset);
	MP_Rect rect = {}; 
	rect.w = debug_point_size;
	rect.h = debug_point_size;
	rect.x = (int)pos_ws.x - rect.w / 2;
	rect.y = (int)pos_ws.y - rect.h / 2;

	mp_set_render_draw_color(c);
	mp_render_fill_rect(&rect);
}

void draw_debug_info(Game_Data& game_data, Font& font, MP_Texture* debug_texture) {
	if (!Globals::toggle_debug_images && !Globals::debug_show_coordinates) { 
		return;
	}

	MP_Renderer* renderer = Globals::renderer;

	if (Globals::toggle_debug_images) {
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
				draw_debug_2d_rotation_matrix_rect({ (float)x + width / 2, (float)y + height / 2 }, &font);
				break;
			}
			case DI_copy_ex: {
				draw_string(font, "rnd_cpy_ex", CT_White, true, title_x, title_y, string_size, true);

				MP_Rect temp_rect = { x, y, width, height };
				static float temp_angle = 0;
				if (key_pressed_and_held(KEY_A)) {
					temp_angle += 1;
				}
				if (key_pressed_and_held(KEY_D)) {
					temp_angle -= 1;
				}
				mp_render_copy_ex(images[IT_Sun].texture, NULL, &temp_rect, temp_angle, NULL, SDL_FLIP_NONE);
				break;
			}
			default: {
				draw_string(font, "Error. Image default case hit in draw debug image function", CT_White, true, x, y, 1, true);
				break;
			}
			}
		}
	}

	if (Globals::debug_show_coordinates) {
		V2 mouse = get_mouse_position(renderer->open_gl.window_handle);
		debug_draw_coor_cs(CT_Green, true, game_data.camera.pos_ws, (int)mouse.x, (int)mouse.y, false);
		debug_draw_coor_cs(CT_Green, true, game_data.camera.pos_ws, (int)game_data.player.pos.x, (int)game_data.player.pos.y, true);
	}

	mp_set_texture_color_mod(font.image.texture, 255, 255, 255);
	mp_set_texture_alpha_mod(debug_texture, 255);
}



void draw_tile(Game_Data& game_data, int tile_index_x, int tile_index_y, float noise_frequency) {
	Image_Type type = IT_Rock_32x32;

	// Move everything around the camera
	int tile_ws_x = (Globals::tile_w * tile_index_x);
	int tile_ws_y = (Globals::tile_h * tile_index_y);

	int tile_cs_x = tile_ws_x - (int)game_data.camera.pos_ws.x;
	int tile_cs_y = tile_ws_y - (int)game_data.camera.pos_ws.y;

	float perlin_x = tile_index_x * noise_frequency;
	float perlin_y = tile_index_y * noise_frequency;
	float perlin = stb_perlin_noise3(perlin_x, perlin_y, 0, 0, 0, 0);

	if (perlin <= -0.2f) {
		type = IT_Water_32x32;
	} else if (perlin > -0.2f && perlin < 0.3f) {
		type = IT_Grass_32x32;
	} else {
		type = IT_Rock_32x32;
	}

	Image* image = get_image(type);

	MP_Rect dst = {tile_cs_x, tile_cs_y, Globals::tile_w, Globals::tile_h};

	mp_render_copy(image->texture, NULL, &dst);
}

void fire_player_arrow(Game_Data& game_data, Image_Type it, int arrow_w, int arrow_h, int speed) {
	Image* image = get_image(it);

	V2 player_cs_pos = convert_ws_to_cs(game_data.player.pos, game_data.camera.pos_ws);

	V2 mouse_cs_pos = get_mouse_position(Globals::renderer->open_gl.window_handle);

	// Calculate the direction vector target <--- origin
	V2 vel = mouse_cs_pos - player_cs_pos;

	// Calculate length
	float length = vel.x * vel.x + vel.y * vel.y;
	length = sqrt(abs(length));

	// Normalize the vel vector
	vel.x = vel.x / length;
	vel.y = vel.y / length;

	Arrow result = create_arrow(image, game_data.player.pos, vel, arrow_w, arrow_h, speed);

	game_data.arrows.push_back(result);
}

void render(Game_Data& game_data) {
	MP_Renderer* renderer = Globals::renderer;

	int window_width = 0;
	int window_height = 0;
	get_window_size(renderer->open_gl.window_handle, window_width, window_height);
	MP_Rect viewport = { 0, 0, window_width, window_height};
	mp_render_set_viewport(&viewport);

	renderer->window_width= window_width;
	renderer->window_height = window_height;

	update_camera(game_data.camera, game_data.player);

	mp_set_render_draw_color(155, 155, 155, 255);

	mp_render_clear();

	// Truncates by default
	int starting_tile_x = (int)game_data.camera.pos_ws.x / Globals::tile_w;
	int starting_tile_y = (int)game_data.camera.pos_ws.y / Globals::tile_h;

	int ending_tile_x = ((int)game_data.camera.pos_ws.x + game_data.camera.w) / Globals::tile_w;
	int ending_tile_y = ((int)game_data.camera.pos_ws.y + game_data.camera.h) / Globals::tile_h;

	// Only render tiles in the view of the player
	// Currently in world space
	// Draw the tiles around the player
	for (int tile_x = starting_tile_x - 1; tile_x < ending_tile_x + 2; tile_x++) {
		for (int tile_y = starting_tile_y - 1; tile_y < ending_tile_y + 2; tile_y++) {
			draw_tile(game_data, tile_x, tile_y, Globals::noise_frequency);
		}
	}

	mp_set_texture_alpha_mod(game_data.player.image->texture, 255);

	// Convert the player's position to camera space
	MP_Rect player_rect = {(int)game_data.player.pos.x, (int)game_data.player.pos.y, game_data.player.w, game_data.player.h};
	V2 player_pos = convert_ws_to_cs({ (float)player_rect.x, (float)player_rect.y }, game_data.camera.pos_ws);
	player_rect.x = (int)player_pos.x - game_data.player.w / 2;
	player_rect.y = (int)player_pos.y - game_data.player.h / 2;
	mp_render_copy(game_data.player.image->texture, NULL, &player_rect);

	Font* font = get_font(game_data.selected_font);
	draw_debug_info(game_data, *font, game_data.player.image->texture);

	for (Arrow& arrow : game_data.arrows) {
		draw_arrow((int)game_data.camera.pos_ws.x, (int)game_data.camera.pos_ws.y, arrow);
		if (Globals::debug_show_coordinates) {
			debug_draw_coor_cs(CT_Green, true, game_data.camera.pos_ws, (int)arrow.pos_ws.x, (int)arrow.pos_ws.y, true);
		}
	}

	mp_render_present();
}

