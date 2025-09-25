#include "Sprite_System.h"

// REVIEW OF COORDINATE SYSTEMS FOR THIS ANIMATION SYSTEM
// PNG Default Origin: Images load with top-left (0,0) as origin via STB (y=0 at top, increasing downward)—rendering would 
// invert vertically without adjustment.

// STB Flip Role: stbi_set_flip_vertically_on_load(true) inverts the buffer during load, swapping rows so buffer y=0 
// is the original bottom (effective bottom-left origin: y increases upward, restoring upright visuals in OpenGL).

// Loop Traversal: Outer loop (x=0 to rows-1) extracts logical rows top-to-bottom; inner loop (y=0 to columns-1) extracts 
// columns left-to-right.

// Src_Rect Y-Inversion Formula: src_rect.y = (image.h / rows) * (rows - 1 - x) compensates for the flipped buffer, mapping 
// x=0 (first row, e.g., North) to the buffer's upper portion (original PNG top), ensuring src_rect points to correct 
// sub-regions without visual flipping.

// Sprite Vector Order: Resulting sprites vector fills in intuitive top-down order (index 0 = top row/North sprites, index 
// rows = next row, etc.)—the flip + inversion duo restores logical sequencing, not inverting the vector itself.

// Global Index for Atlas: global_index = row * columns + current_frame_index pulls the right sprite 
// (e.g., FD_SW row=5 → index 5*columns + frame, corresponding to original SW position).

// Rendering Alignment: With bottom-left NDC/UV, this setup renders upright; if inverted, add y-flip in vertex shader 
// (f_texture_coordinates.y = 1.0 - v_texture_coordinates.y).

std::unordered_map<std::string, Sprite_Sheet_Data> sprite_sheet_data_map;

/*
Sprite_Sheet_Data dummy_sprite_sheet_data = {
	// std::string root; std::string sprite_sheet_name; std::string ext; int rows; int columns;
	"assets\\", "dummy_image", ".png", 1, 1
};

Sprite_Sheet_Data* get_sprite_sheet_data(std::string name) {
	auto it = sprite_sheet_map.find(name);

	if (it != sprite_sheet_map.end()) {
		return &it->second;
	}
	else {
		// Return bad data
		return &dummy_sprite_sheet_data;
	}
}
*/

Sprite create_sprite(Image image, MP_Rect src_rect) {
	Sprite result = {};

	result.image = image;
	result.src_rect = src_rect;

	return result; 
}

Sprite_Sheet create_animation_sprite_sheet(std::string full_file_path, int rows, int columns) {
	Sprite_Sheet result = {};
	result.rows = rows;
	result.columns = columns;

	Image image = load_image(full_file_path.c_str());

	if (full_file_path == "assets\\ravenous_skulk\\walking\\ravenous_skulk_walking_atlas.png") {
		int i = 0;
		i++;
	}

	if (image.texture != NULL) {
		for (int x = 0; x < rows; x++) {
			for (int y = 0; y < columns; y++) {
				MP_Rect src_rect = {};

				src_rect.x = (y * (image.w / columns));
				src_rect.y = (image.h / rows) * (rows - 1 - x);
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

std::string get_sprite_sheet_name(const std::string_view entity_name, Animation_State as) {
	std::string result = {};
	switch (as) {
	case AS_Idle:				
		result = std::string(entity_name) + "_idle";
		break;
	case AS_Walking:			
		result = std::string(entity_name) + "_walking";
		break;
	case AS_Running:			
		result = std::string(entity_name) + "_running";
		break;
	case AS_Attacking:			
		result = std::string(entity_name) + "_attacking";
		break;
	case AS_Dying:				
		result = std::string(entity_name) + "_dying";
		break;
	case AS_No_Animation:		
		result = std::string(entity_name);
		break;
	default: 
		return "dummy_image";
	}

	return result;
}

std::string get_sprite_sheet_atlas_name(const std::string_view entity_name, Animation_State as) {
	return get_sprite_sheet_name(entity_name, as) + "_atlas";
}

std::string get_facing_direction_sprite_sheet_name(const std::string& entity_name, Facing_Direction fd, Animation_State as) {
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
	case AS_Dying: {
		result = entity_name + "_dying";
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
		case FD_N: result += "_n"; break;
		case FD_NNE: result += "_nne"; break;
		case FD_NE: result += "_ne"; break;
		case FD_ENE: result += "_ene"; break;
		case FD_E: result += "_e"; break;
		case FD_ESE: result += "_ese"; break;
		case FD_SE: result += "_se"; break;
		case FD_SSE: result += "_sse"; break;
		case FD_S: result += "_s"; break;
		case FD_SSW: result += "_ssw"; break;
		case FD_SW: result += "_sw"; break;
		case FD_WSW: result += "_wsw"; break;
		case FD_W: result += "_w"; break;
		case FD_WNW: result += "_wnw"; break;
		case FD_NW: result += "_nw"; break;
		case FD_NNW: result += "_nnw"; break;
		case FD_NONE:; break;
		default: result += "_unk"; break;
	}

	return result;
}

std::string get_torso_facing_direction_sprite_sheet_name(const std::string& entity_name, Facing_Direction fd, Animation_State as) {
	return get_facing_direction_sprite_sheet_name(entity_name, fd, as) + "_torso";
}

std::string get_legs_facing_direction_sprite_sheet_name(const std::string& entity_name, Facing_Direction fd_torso/*, V2 legs_moving_direction*/, Animation_State as) {
	return get_facing_direction_sprite_sheet_name(entity_name, fd_torso, as) + "_legs";
}

Facing_Direction get_facing_direction_8(V2 vec) {
	float angle = calculate_facing_direction_east_counterclockwise(vec);

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

Facing_Direction get_facing_direction_8_atlas(V2 vec) {
    float angle = calculate_facing_direction_north_clockwise(vec);
    Facing_Direction result = FD_NONE;
    if (angle >= 337.5f || angle < 22.5f) {
        result = FD_N;
    }
    else if (angle >= 22.5f && angle < 67.5f) {
        result = FD_NE;
    }
    else if (angle >= 67.5f && angle < 112.5f) {
        result = FD_E;
    }
    else if (angle >= 112.5f && angle < 157.5f) {
        result = FD_SE;
    }
    else if (angle >= 157.5f && angle < 202.5f) {
        result = FD_S;
    }
    else if (angle >= 202.5f && angle < 247.5f) {
        result = FD_SW;
    }
    else if (angle >= 247.5f && angle < 292.5f) {
        result = FD_W;
    }
    else if (angle >= 292.5f && angle < 337.5f) {
        result = FD_NW;
    }
    return result;
}

Facing_Direction get_facing_direction_16(V2 vec) {
	float angle = calculate_facing_direction_east_counterclockwise(vec);
	Facing_Direction result = FD_NONE;
	if (angle >= 348.75f || angle < 11.25f) {
		result = FD_E;
	} else if (angle >= 11.25f && angle < 33.75f) {
		result = FD_ENE;
	} else if (angle >= 33.75f && angle < 56.25f) {
		result = FD_NE;
	} else if (angle >= 56.25f && angle < 78.75f) {
		result = FD_NNE;
	} else if (angle >= 78.75f && angle < 101.25f) {
		result = FD_N;
	} else if (angle >= 101.25f && angle < 123.75f) {
		result = FD_NNW;
	} else if (angle >= 123.75f && angle < 146.25f) {
		result = FD_NW;
	} else if (angle >= 146.25f && angle < 168.75f) {
		result = FD_WNW;
	} else if (angle >= 168.75f && angle < 191.25f) {
		result = FD_W;
	} else if (angle >= 191.25f && angle < 213.75f) {
		result = FD_WSW;
	} else if (angle >= 213.75f && angle < 236.25f) {
		result = FD_SW;
	} else if (angle >= 236.25f && angle < 258.75f) {
		result = FD_SSW;
	} else if (angle >= 258.75f && angle < 281.25f) {
		result = FD_S;
	} else if (angle >= 281.25f && angle < 303.75f) {
		result = FD_SSE;
	} else if (angle >= 303.75f && angle < 326.25f) {
		result = FD_SE;
	} else if (angle >= 326.25f && angle < 348.75f) {
		result = FD_ESE;
	}
	return result;
}

bool has_animation_state_changed(Animation_Tracker* at, const std::string& entity_name, Animation_State new_as, Animation_Mode new_mode) {
	if (entity_name != at->entity_name ||
		at->as != new_as || 
		at->mode != new_mode) {
		return true;
	}
	return false;
}

// Core function; refactored to reduce duplication.
void change_animation_tracker(Animation_Tracker* at, const std::string& entity_name, Animation_State new_as, Animation_Play_Speed aps, 
	Animation_Mode mode, bool flip_horizontally, std::optional<V2> velocity_opt) {

    if (!at) {
        log("Error: Null Animation_Tracker provided.");
        return;
    }
	// The bug is that it's setting it to run the frame once, then the frame players, and sets it to static first frame, but detects 
	// that it's different in the has changed, and resets it to play the loop again. I need a way to track if it's trying to paly once 
	// nad not change animation state again or something like that.

    bool state_changed = has_animation_state_changed(at, entity_name, new_as, mode);
    V2 velocity = velocity_opt.value_or(V2{0.0f, 0.0f}); 

    bool velocity_changed = (velocity_opt && (velocity.x != at->last_velocity.x || velocity.y != at->last_velocity.y));
    if (!state_changed && !velocity_changed) {
        return;  
    }

    // Update velocity if provided.
    if (velocity_opt) {
        at->last_velocity = velocity;
    }

    // Compute new facing direction based on type.
    Facing_Direction new_fd = at->fd;  // Default to current.
    switch (at->att) {
        case ATT_Direction_2:
            at->flip_horizontally = flip_horizontally;
            at->selected_sprite_sheet = get_sprite_sheet_name(entity_name, new_as);
            break;
        case ATT_Direction_8:
            new_fd = get_facing_direction_8(velocity);
            at->selected_sprite_sheet = get_facing_direction_sprite_sheet_name(entity_name, new_fd, new_as);
            break;
        case ATT_Direction_8_Legs:
            new_fd = get_facing_direction_8(velocity);
            at->selected_sprite_sheet = get_legs_facing_direction_sprite_sheet_name(entity_name, new_fd, new_as);
            break;
		case ATT_Direction_8_Atlas: 
			new_fd = get_facing_direction_8_atlas(velocity);
			at->selected_sprite_sheet = get_sprite_sheet_atlas_name(entity_name, new_as);
			break;
        case ATT_Direction_16:
            new_fd = get_facing_direction_16(velocity);
            at->selected_sprite_sheet = get_facing_direction_sprite_sheet_name(entity_name, new_fd, new_as);
            break;
        case ATT_Direction_16_Torso:
            new_fd = get_facing_direction_16(velocity);
            at->selected_sprite_sheet = get_torso_facing_direction_sprite_sheet_name(entity_name, new_fd, new_as);
            break;
        default:
            log("Error: Unhandled Animation_Tracker_Type in update_animation_direction.");
            return;
    }

    // Update facing direction if changed.
    if (new_fd != at->fd) {
        at->fd = new_fd;
    }

    // Reset state if any change occurred.
    if (state_changed || new_fd != at->fd) {
		at->entity_name = entity_name;
		at->as = new_as;
		at->current_frame_index = 0;
		// TODO: Change this to be apart of the csv file
		at->aps = aps;
		at->mode = mode;
		if (new_as == AS_Dying) {
			at->loops = false;
		}
    }
}

Animation_Tracker create_animation_tracker(Animation_Tracker_Type att, std::string_view entity_name, 
	Animation_State starting_as, Animation_Play_Speed starting_aps, Animation_Mode starting_mode, bool loops) {
	Animation_Tracker result = {};

	result.att = att;
	result.entity_name = std::string(entity_name);
	result.as = starting_as;
	result.aps = starting_aps;
	result.mode = starting_mode;
	// TODO: Merge this into the actual function to take the att
	if (result.att == ATT_Direction_8_Atlas) {
		result.selected_sprite_sheet = get_sprite_sheet_atlas_name(entity_name, starting_as);
	}
	else {
		result.selected_sprite_sheet = get_sprite_sheet_name(entity_name, starting_as);
	}
	result.current_frame_index = 0;
	result.current_frame_time = 0.0f;
	result.loops = loops;

	return result;
}

void update_animation_tracker(Animation_Tracker* at, float delta_time, float speed_based) {
    Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);
	// The number of frames is the columns
	size_t frame_count = ss->columns;

    // Early exit for no animation or single-frame sheets
    if (at->mode == AM_No_Animation || frame_count <= 1) {
        at->current_frame_index = 0;  // Or keep current index if pausing is desired
        return;
    }

    if (at->mode == AM_Static_First_Frame) {
        at->current_frame_index = 0;
        return;  // No timer updates needed
    }

    if (at->mode == AM_Static_Last_Frame) {
        at->current_frame_index = (int)frame_count - 1;
        return;  // No timer updates needed
    }

    // Handle animating modes
    if (at->current_frame_time <= 0.0f) {
        // Set frame time based on speed
        switch (at->aps) {
            case APS_Slow:
                at->current_frame_time = 1.0f / Globals::slow_frames_per_sec;
                break;
            case APS_Fast:
                at->current_frame_time = 1.0f / Globals::fast_frames_per_sec;
                break;
            case APS_Speed_Based:
                at->current_frame_time = 1.0f / (speed_based / 25.0f);
                break;
            default:
                at->current_frame_time = 1.0f;  // Fallback
        }

        // Advance frame
		if (at->mode == AM_Animate_Looping) {
			at->current_frame_index = (at->current_frame_index + 1) % frame_count;
		} else if (at->mode == AM_Animate_Looping_Reversed) {
            at->current_frame_index = at->current_frame_index = (at->current_frame_index == 0 ? (int)frame_count - 1 : at->current_frame_index - 1) % (int)frame_count;
        } else if (at->mode == AM_Animate_Once) {
            if (at->current_frame_index < frame_count - 1) {
                at->current_frame_index++;
            } else {
                // Optionally transition to static or another mode
                at->mode = AM_Static_Last_Frame;
            }
        }
    } else {
        at->current_frame_time -= delta_time;
    }
}

int get_atlas_row_index_from_facing_direction_8(Facing_Direction fd) {
    switch (fd) {
        case FD_N:  return 0;
        case FD_NE: return 1;
        case FD_E:  return 2;
        case FD_SE: return 3;
        case FD_S:  return 4;
        case FD_SW: return 5;
        case FD_W:  return 6;
        case FD_NW: return 7;
        default:	return 0;
    }
}

const int ATLAS_ROWS_8_DIRECTIONS = 8;

// Takes into account texture atlas
int get_animation_tracker_current_frame_index(Animation_Tracker* at, int ss_rows, int ss_stride) {
	int result = -1;
	if (ss_rows == ATLAS_ROWS_8_DIRECTIONS && at->att == ATT_Direction_8_Atlas) {
		int selected_row = get_atlas_row_index_from_facing_direction_8(at->fd);
		result = selected_row * ss_stride + at->current_frame_index;
	}
	else {
		result = at->current_frame_index;
	}

	return result;
}

void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst, float angle, uint8_t alpha_mod) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	MP_Rect src = ss->sprites[get_animation_tracker_current_frame_index(at, ss->rows, ss->columns)].src_rect;
	MP_Texture* texture = ss->sprites[get_animation_tracker_current_frame_index(at, ss->rows, ss->columns)].image.texture;

	mp_set_texture_alpha_mod(texture, alpha_mod);

	if (at->flip_horizontally == false) {
		mp_render_copy_ex(texture, &src, &dst, angle, NULL, SDL_FLIP_NONE);
	}
	else {
		mp_render_copy_ex(texture, &src, &dst, angle, NULL, SDL_FLIP_HORIZONTAL);
	}

	mp_set_texture_alpha_mod(texture, 255);
}

void draw_animation_tracker(Animation_Tracker* at, MP_Rect dst, float angle) {
	draw_animation_tracker(at, dst, angle, 255);
}

void draw_animation_tracker_outlined(Animation_Tracker* at, MP_Rect dst, float angle, Color_Type ct, float outline_thickness) {
	Sprite_Sheet* ss = get_sprite_sheet(at->selected_sprite_sheet);

	int index = get_animation_tracker_current_frame_index(at, ss->rows, ss->columns);
	MP_Rect src = ss->sprites[index].src_rect;
	MP_Texture* texture = ss->sprites[index].image.texture;

	Color_RGBA color_rgba = get_color_type(ct);
	Color_4F color_4f_normalized = {(float)color_rgba.r / 255.0f, (float)color_rgba.g / 255.0f, (float)color_rgba.b / 255.0f, (float)color_rgba.a / 255.0f};
	if (at->flip_horizontally == false) {
		// mp_render_copy_ex(texture, &src, &dst, angle, NULL, SDL_FLIP_NONE);
		mp_render_copy_outlined_ex(texture, &src, &dst, angle, NULL, SDL_FLIP_NONE, color_4f_normalized, outline_thickness);
	}
	else {
		mp_render_copy_outlined_ex(texture, &src, &dst, angle, NULL, SDL_FLIP_HORIZONTAL, color_4f_normalized, outline_thickness);
	}
}

Type_Descriptor sprite_sheet_type_descriptors[] = {
	FIELD(Sprite_Sheet_Data, VT_String, root),
	FIELD(Sprite_Sheet_Data, VT_String, sprite_sheet_name),
	FIELD(Sprite_Sheet_Data, VT_String, ext),
	FIELD(Sprite_Sheet_Data, VT_Int, rows),
	FIELD(Sprite_Sheet_Data, VT_Int, columns)
};

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
