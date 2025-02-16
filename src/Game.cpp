#include "Game.h"

#define STB_PERLIN_IMPLEMENTATION
#include <perlin.h>

// Camera's position is relative to the player
void update_camera(Camera& camera, Player& player) {
	camera.pos_ws.x = (player.rb.pos_ws.x - (Globals::resolution_x / 2.0f));
	camera.pos_ws.y = (player.rb.pos_ws.y - (Globals::resolution_y / 2.0f));

	camera.w = Globals::resolution_x;
	camera.h = Globals::resolution_y;
}

Camera create_camera(Player& player) {
	Camera result = {};

	update_camera(result, player);

	return result;
}

int debug_point_size = 6;
Font_Type debug_font = FT_Basic;
// The draw_at variable should be in world space. World space is technically UI space.
void debug_draw_coor(Game_Data& game_data, V2 coor_to_draw, bool convert_coor_to_draw_to_cs,
	V2 draw_at, bool convert_draw_at_to_cs, Color_Type c, bool background, std::string custom_text) {

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

	std::string str = custom_text + "x = " + std::to_string((int)coor_to_draw_final.x) + ", y = " 
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
		collider_ws_pos = convert_ws_to_cs(collider_ws_pos, game_data.camera.pos_ws);
		debug_draw_coor(
			game_data, rb.pos_ws, false, collider_ws_pos, false, CT_Dark_Yellow, true, "CS: "
		);
	}
}

void draw_debug_info(Game_Data& game_data, Font& font, MP_Texture* debug_texture, float delta_time) {
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
				draw_debug_2d_rotation_matrix_rect({ (float)x + width / 2, (float)y + height / 2 }, &font, delta_time);
				break;
			}
			case DI_copy_ex: {
				draw_string(font, "rnd_cpy_ex", CT_White, true, title_x, title_y, string_size, true);

				MP_Rect temp_rect = { x, y, width, height };
				static float temp_angle = 0;
				static float rotation_speed = 8.0f;
				temp_angle += delta_time * rotation_speed;
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
		// The mouse position is already in camera space
		debug_draw_coor(game_data, mouse, false, mouse, false, CT_Green, true, "Mouse: ");
		debug_draw_coor(game_data, game_data.player.rb.pos_ws, false, 
			{(float)Globals::resolution_x / 2, (float)Globals::resolution_y / 2}, false,
			CT_Green, true, "Player WS Pos: ");
	}

	if (Globals::debug_show_stats) {
		int stats_string_size = 2;
		int stats_border_padding = 50;
		int stats_w = 300;
		int stats_h = 600;
		int stats_rect_x = Globals::resolution_x - (stats_w + stats_border_padding);
		int stats_rect_y = Globals::resolution_y - (stats_h + stats_border_padding);
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

void render(Game_Data& game_data, float delta_time) {
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

	draw_player(game_data.player, game_data.camera.pos_ws);
	draw_colliders(&game_data.player.rb, game_data.camera.pos_ws);
	debug_draw_collider_coodinates(game_data, game_data.player.rb);
	draw_circle_cs(CT_Red, game_data.player.rb.pos_ws, game_data.camera.pos_ws, (int)game_data.player.image->sprite_radius, 20.0);

	Font* font = get_font(game_data.selected_font);
	draw_debug_info(game_data, *font, game_data.player.image->texture, delta_time);

	for (Handle projectile: game_data.projectile_handles) {
		Projectile* p = get_entity_pointer_from_handle(game_data.projectile_storage, projectile);
		if (p == NULL) {
			log("Error: handle returned null");
			continue;
		}
		draw_projectile((int)game_data.camera.pos_ws.x, (int)game_data.camera.pos_ws.y, *p);
		if (Globals::debug_show_coordinates) {
			debug_draw_coor(game_data, p->rb.pos_ws, false, p->rb.pos_ws, true, 
				CT_Green, true, "WS Pos: ");
		}
	}

	for (Handle zombie_handle: game_data.enemy_unit_handles) {
		Unit* u = get_entity_pointer_from_handle(game_data.unit_storage, zombie_handle);
		if (u == NULL) {
			log("Error: handle returned null");
			continue;
		}
		draw_unit(*u, game_data.camera.pos_ws);
		draw_colliders(&u->rb, game_data.camera.pos_ws);
	}

	mp_render_present();
}

