#include "Sprite_Sheet.h"

Sprite create_sprite(Image image, MP_Rect src_rect) {
	Sprite result = {};

	result.image = image;
	result.src_rect = src_rect;

	return result; 
}

Sprite_Sheet create_animation_sprite_sheet(std::string full_file_path, float default_frame_speed_seconds, 
	int rows, int columns) {
	Sprite_Sheet result = {};
	result.default_frame_speed_seconds = default_frame_speed_seconds;
	Image image = load_image(full_file_path.c_str());

	if (image.texture != NULL) {
		for (int x = 0; x < rows; x++) {
			for (int y = 0; y < columns; y++) {
				MP_Rect src_rect = {};

				src_rect.x = (y * image.h);
				src_rect.y = (x * image.w);
				src_rect.w = (image.w / columns);
				src_rect.h = (image.h / rows);

				Sprite new_sprite = create_sprite(image, src_rect);

				result.sprites.push_back(new_sprite);
			}
		}
	}

	return result;
}

Sprite_Sheet dummy_sprite_sheet = {};
std::unordered_map<std::string, Sprite_Sheet> sprite_sheet_map;

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
	result.current_frame_index = 0;
	result.current_frame_time = 0.0f;

	return result;
}

void change_animation(Animation_Tracker* at, std::string new_selected_sprite_sheet, bool flip) {
	// Init the default frame to zero here
	if (at->selected_sprite_sheet != new_selected_sprite_sheet) {
		at->selected_sprite_sheet = new_selected_sprite_sheet;
		at->current_frame_index = 0;
	}
	if (at->flip != flip) {
		at->flip = !at->flip;
	}
}

void update_animation_tracker(Animation_Tracker* at, float delta_time) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	size_t frame_count = ss->sprites.size();

	// Only update if there is more than 1 image present
	if (frame_count > 1) {
		if (0.0f >= at->current_frame_time) {
			at->current_frame_time = ss->default_frame_speed_seconds;
			int index = at->current_frame_index;
			// Increment to next frame
			at->current_frame_index = (index + 1) % frame_count;
		}
		else {
			at->current_frame_time -= delta_time;
		}
	}
}

void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	MP_Rect src = ss->sprites[at->current_frame_index].src_rect;
	MP_Texture* texture = ss->sprites[at->current_frame_index].image.texture;

	if (at->flip) {
		mp_render_copy_ex(texture, &src, &dst, 0, NULL, SDL_FLIP_HORIZONTAL);
	}
	else {
		mp_render_copy_ex(texture, &src, &dst, 0, NULL, SDL_FLIP_NONE);
	}
}

Type_Descriptor sprite_sheet_type_descriptors[] = {
	FIELD(Sprite_Sheet_Data, VT_String, root),
	FIELD(Sprite_Sheet_Data, VT_String, image_name),
	FIELD(Sprite_Sheet_Data, VT_String, ext),
	FIELD(Sprite_Sheet_Data, VT_Float, default_frame_speed_seconds),
	FIELD(Sprite_Sheet_Data, VT_Int, rows),
	FIELD(Sprite_Sheet_Data, VT_Int, columns)
};

std::unordered_map<std::string, Sprite_Sheet_Data> sprite_sheet_data_map;

void load_sprite_sheet_csv(CSV_Data* data) {
	std::vector<Sprite_Sheet_Data> sprite_sheet_data = {};
	sprite_sheet_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_sprite_sheet_decsriptors(sprite_sheet_type_descriptors);

	load_csv_data_file(data, (char*)sprite_sheet_data.data(), sprite_sheet_type_descriptors, sizeof(Sprite_Sheet_Data));

	for (Sprite_Sheet_Data& it : sprite_sheet_data) {
		sprite_sheet_data_map[it.image_name] = it;
	}
}

void load_sprite_sheets() {
	dummy_sprite_sheet = create_animation_sprite_sheet("assets\\dummy_image.png", 0.25, 1, 1);
	sprite_sheet_map["temp_zombie_walk"] = create_animation_sprite_sheet("assets\\temp_zombie_walk.png", 0.25, 1, 10);
	sprite_sheet_map["idle_zombie_male"] = create_animation_sprite_sheet("assets\\idle_temp_zombie.png", NULL, 1, 1);

	for (const auto& it : sprite_sheet_data_map) {
		std::string full_file_path = it.second.root + it.second.image_name + ".png";

		sprite_sheet_map[it.second.image_name] = create_animation_sprite_sheet(
			full_file_path.c_str(), 
			it.second.default_frame_speed_seconds, 
			it.second.rows, 
			it.second.columns
		);
	}
}
