#include "Sprite_Sheet.h"

Sprite create_sprite(Image* image, MP_Rect src_rect) {
	Sprite result = {};

	result.image = image;
	result.src_rect = src_rect;

	return result; 
}

Sprite_Sheet create_animation_sprite_sheet(std::string image_name, float default_frame_speed_seconds, 
	int rows, int columns) {
	Sprite_Sheet result = {};
	result.default_frame_speed = default_frame_speed_seconds;
	Image* image = get_image(image_name);

	if (image != NULL) {
		for (int x = 0; x < rows; x++) {
			for (int y = 0; y < columns; y++) {
				MP_Rect src_rect = {};

				src_rect.x = (y * image->h);
				src_rect.y = (x * image->w);
				src_rect.w = (image->w / columns);
				src_rect.h = (image->h / rows);

				Sprite new_sprite = create_sprite(image, src_rect);

				result.sprites.push_back(new_sprite);
			}
		}
	}

	return result;
}

Sprite_Sheet dummy_sprite_sheet = {};
std::unordered_map<std::string, Sprite_Sheet> sprite_sheet_map;

void load_sprite_sheets() {
	dummy_sprite_sheet = create_animation_sprite_sheet("dummy_image", 1, 1, 1);
	sprite_sheet_map["temp_zombie_walk"] = create_animation_sprite_sheet("temp_zombie_walk", 1, 1, 10);
}

Sprite_Sheet* get_sprite_sheet(std::string name) {
	auto it = sprite_sheet_map.find(name);

	if (it != sprite_sheet_map.end()) {
		return &it->second;
	}
	else {
		// Return bad data
		return &dummy_sprite_sheet;
	}
}

Animation_Tracker create_animation_tracker(std::string selected_sprite_sheet) {
	Animation_Tracker result = {};

	result.selected_sprite_sheet = selected_sprite_sheet;

	return result;
}

void change_animation(Animation_Tracker* at, std::string new_sprite_sheet) {
	REF(at);
	REF(new_sprite_sheet);
	// Init the default frame to zero here
}

void update_animation_tracker(Animation_Tracker* at, float delta_time) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	size_t frame_count = ss->sprites.size();

	if (0.0f > at->current_frame_time) {
		at->current_frame_time = ss->default_frame_speed;
		int index = at->current_frame_index;
		// Increment to next frame
		at->current_frame_index = (index + 1) % frame_count;
	}
	else {
		at->current_frame_time -= delta_time;
	}
}

void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	MP_Rect src = ss->sprites[at->current_frame_index].src_rect;
	MP_Texture* texture = ss->sprites[at->current_frame_index].image->texture;
	mp_render_copy_ex(texture, &src, &dst, 0, NULL, SDL_FLIP_NONE);
}
