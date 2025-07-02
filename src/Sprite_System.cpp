#include "Sprite_System.h"

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

				src_rect.x = (y * (image.w / columns));
				src_rect.y = (x * (image.h / rows));
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

std::string get_sprite_sheet_name(std::string entity_name, Animation_State as) {
	std::string result = {};

	switch (as) {
	case AS_Idle: {
		result = entity_name + "_idle";
		break;
	}
	case AS_Walking: {
		result = entity_name + "_walking";
		break;
	}
	case AS_Running: {
		result = entity_name + "_running";
		break;
	}
	case AS_Attacking: {
		result = entity_name + "_attacking";
		break;
	}
	case AS_Death: {
		result = entity_name + "_death";
		break;
	}
	case AS_No_Animation: {
		result = entity_name;
		break;
	}
	default: {
		// Just return the dummy image if case is not found
		result = "dummy_image";
		break;
	}
	}

	return result;
}


std::string get_sprite_sheet_name_direction_8(std::string entity_name, Facing_Direction fd, Animation_State as) {
	std::string result = {};

	switch (as) {
	case AS_Idle: {
		result = entity_name + "_idle";
		break;
	}
	case AS_Walking: {
		result = entity_name + "_walking";
		break;
	}
	case AS_Running: {
		result = entity_name + "_running";
		break;
	}
	case AS_Attacking: {
		result = entity_name + "_attacking";
		break;
	}
	case AS_Death: {
		result = entity_name + "_death";
		break;
	}
	case AS_No_Animation: {
		result = entity_name;
		break;
	}
	default: {
		// Just return the dummy image if case is not found
		result = "dummy_image";
		break;
	}
	}

	switch (fd) {
		case FD_N:      result += "_n";      break;
		// case FD_NNE:    result += "_NNE";    break;
		case FD_NE:     result += "_ne";     break;
		// case FD_ENE:    result += "_ENE";    break;
		case FD_E:      result += "_e";      break;
		// case FD_ESE:    result += "_ESE";    break;
		case FD_SE:     result += "_se";     break;
		// case FD_SSE:    result += "_SSE";    break;
		case FD_S:      result += "_s";      break;
		// case FD_SSW:    result += "_SSW";    break;
		case FD_SW:     result += "_sw";     break;
		// case FD_WSW:    result += "_WSW";    break;
		case FD_W:      result += "_w";      break;
		// case FD_WNW:    result += "_WNW";    break;
		case FD_NW:     result += "_nw";     break;
		// case FD_NNW:    result += "_NNW";    break;
		case FD_NONE: ;break;
		default:        result += "_unk";    break;
		}

	return result;
}

void change_animation(Animation_Tracker* at, std::string entity_name, Animation_State new_as, 
	Facing_Direction facing_direction, Animation_Play_Speed animation_play_speed) {
	if (at->as != new_as || entity_name != at->entity_name) {
		at->entity_name = entity_name;
		at->as = new_as;
		at->selected_sprite_sheet = get_sprite_sheet_name(at->entity_name, new_as);
		at->current_frame_index = 0;
		// TODO: Change this to be apart of the csv file
		at->aps = animation_play_speed;
		if (new_as == AS_Death) {
			at->loops = false;
		}
	}
	at->fd = facing_direction;
}

Facing_Direction get_facing_direction_8(V2 vec) {
	float angle = calculate_facing_direction(vec);

	Facing_Direction result = FD_NONE;

	if (angle >= 337.5f || angle < 22.5f) {
		result = FD_E;
	}
	else if (angle >= 22.5f && angle < 67.5f) {
		result = FD_NE;
	}
	else if (angle >= 67.5f && angle < 112.5f) {
		result = FD_N;
	}
	else if (angle >= 112.5f && angle < 157.5f) {
		result = FD_NW;
	}
	else if (angle >= 157.5f && angle < 202.5f) {
		result = FD_W;
	}
	else if (angle >= 202.5f && angle < 247.5f) {
		result = FD_SW;
	}
	else if (angle >= 247.5f && angle < 292.5f) {
		result = FD_S;
	}
	else if (angle >= 292.5f && angle < 337.5f) {
		result = FD_SE;
	}

	return result;
}

void change_animation_direction_8(Animation_Tracker* at, std::string entity_name, Animation_State new_as, 
	V2 velocity, Animation_Play_Speed aps) {
	if (velocity.x != at->last_velocity.x || velocity.y != at->last_velocity.y 
		|| at->as != new_as 
		|| entity_name != at->entity_name) {
		at->last_velocity = velocity;
		Facing_Direction new_fd = get_facing_direction_8(velocity);
		if (at->fd != new_fd) {
			at->fd = new_fd;
			at->entity_name = entity_name;
			at->as = new_as;
			at->selected_sprite_sheet = get_sprite_sheet_name_direction_8(at->entity_name, at->fd, new_as);
			at->current_frame_index = 0;
			// TODO: Change this to be apart of the csv file
			at->aps = aps;
			if (new_as == AS_Death) {
				at->loops = false;
			}
		}
	}
}

Animation_Tracker create_animation_tracker(std::string entity_name, Animation_State starting_as, bool loops) {
	Animation_Tracker result = {};

	result.entity_name = entity_name;
	result.as = starting_as;
	result.selected_sprite_sheet = get_sprite_sheet_name(entity_name, starting_as);
	result.current_frame_index = 0;
	result.current_frame_time = 0.0f;
	result.loops = loops;

	return result;
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
				break;
			}
			default: {
				// 1 is enough to recognize there is a bug
				at->current_frame_time = 1.0f;
			}
			}

			int index = at->current_frame_index;
			if (at->loops == true) {
				// Increment to next frame
				at->current_frame_index = (index + 1) % frame_count;
			}
			else {
				if (at->current_frame_index < frame_count - 1) {
					at->current_frame_index++;
				}
			}
		}
		else {
			at->current_frame_time -= delta_time;
		}
	}
}

void update_animation_tracker(Animation_Tracker* at, float delta_time) {
	update_animation_tracker(at, delta_time, NULL);
}

void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst, float angle) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	MP_Rect src = ss->sprites[at->current_frame_index].src_rect;
	MP_Texture* texture = ss->sprites[at->current_frame_index].image.texture;

	mp_render_copy_ex(texture, &src, &dst, angle, NULL, SDL_FLIP_NONE);
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
		if (it.second.sprite_sheet_name == "") {
			continue;
		}
		std::string full_file_path = it.second.root + it.second.sprite_sheet_name + it.second.ext;

		sprite_sheet_map[it.second.sprite_sheet_name] = create_animation_sprite_sheet(
			full_file_path.c_str(), 
			it.second.rows, 
			it.second.columns
		);
	}
}
