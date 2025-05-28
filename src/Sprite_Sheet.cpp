#include "Sprite_Sheet.h"

Sprite create_sprite(Image image, MP_Rect src_rect) {
	Sprite result = {};

	result.image = image;
	result.src_rect = src_rect;

	return result; 
}

Sprite_Sheet create_animation_sprite_sheet(std::string full_file_path, int rows, int columns) {
	Sprite_Sheet result = {};
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

void change_animation(Animation_Tracker* at, std::string new_selected_sprite_sheet, 
	Facing_Direction facing_direction, Animation_Play_Speed animation_play_speed) {
	// Init the default frame to zero here
	if (at->selected_sprite_sheet != new_selected_sprite_sheet) {
		at->selected_sprite_sheet = new_selected_sprite_sheet;
		at->current_frame_index = 0;
		at->aps = animation_play_speed;
	}
	at->fd = facing_direction;
}

void update_animation_tracker(Animation_Tracker* at, float delta_time, float speed) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	size_t frame_count = ss->sprites.size();

	// Only update if there is more than 1 image present
	if (frame_count > 1) {
		if (0.0f >= at->current_frame_time) {
			switch (at->aps) {
			case APS_Slow: {
				at->current_frame_time = 1.0f / Globals::slow_frames_per_sec;
				break;
			}
			case APS_Fast: {
				at->current_frame_time = 1.0f / Globals::fast_frames_per_sec;
				break;
			}
			case APS_Speed_Based: {
				// Speed Based
				at->current_frame_time = 1.0f / (speed / 25.0f);
				std::string str = std::to_string(at->current_frame_time);
				log(str.c_str());
				break;
			}
			default: {
				// 1 is enough to recognize there is a bug
				at->current_frame_time = 1.0f;
			}
			}
			int index = at->current_frame_index;
			// Increment to next frame
			at->current_frame_index = (index + 1) % frame_count;
		}
		else {
			at->current_frame_time -= delta_time;
		}
	}
}

void update_animation_tracker(Animation_Tracker* at, float delta_time) {
	update_animation_tracker(at, delta_time, NULL);
}

void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	MP_Rect src = ss->sprites[at->current_frame_index].src_rect;
	MP_Texture* texture = ss->sprites[at->current_frame_index].image.texture;

	if (at->fd == FD_Left) {
		mp_render_copy_ex(texture, &src, &dst, 0, NULL, SDL_FLIP_HORIZONTAL);
	}
	else {
		mp_render_copy_ex(texture, &src, &dst, 0, NULL, SDL_FLIP_NONE);
	}
}

Type_Descriptor sprite_sheet_type_descriptors[] = {
	FIELD(Sprite_Sheet_Data, VT_String, root),
	FIELD(Sprite_Sheet_Data, VT_String, sprite_sheet_name),
	FIELD(Sprite_Sheet_Data, VT_String, ext),
	FIELD(Sprite_Sheet_Data, VT_Int, rows),
	FIELD(Sprite_Sheet_Data, VT_Int, columns)
};

std::unordered_map<std::string, Sprite_Sheet_Data> sprite_sheet_data_map;

void load_sprite_sheet_data_csv(CSV_Data* data) {
	std::vector<Sprite_Sheet_Data> sprite_sheet_data = {};
	sprite_sheet_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_sprite_sheet_decsriptors(sprite_sheet_type_descriptors);

	load_csv_data_file(data, (char*)sprite_sheet_data.data(), sprite_sheet_type_descriptors, sizeof(Sprite_Sheet_Data));

	for (Sprite_Sheet_Data& it : sprite_sheet_data) {
		sprite_sheet_data_map[it.sprite_sheet_name] = it;
	}
}

void load_sprite_sheets() {
	dummy_sprite_sheet = create_animation_sprite_sheet("assets\\dummy_image.png", 1, 1);

	for (const auto& it : sprite_sheet_data_map) {
		std::string full_file_path = it.second.root + it.second.sprite_sheet_name+ ".png";

		sprite_sheet_map[it.second.sprite_sheet_name] = create_animation_sprite_sheet(
			full_file_path.c_str(), 
			it.second.rows, 
			it.second.columns
		);
	}
}
