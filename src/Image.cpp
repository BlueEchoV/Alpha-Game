#include "Image.h"

Image load_image(const char* file_path) {
	Image result = {};

	// Might be necessary for open gl, testing soon
	stbi_set_flip_vertically_on_load(true); 

	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);
	if (data == NULL) {
		log("Error: stbi_load is null: %s", file_path);
	}

	// Get the radius from the center of the image 
	// to the farthest non transparent pixel in the image
	V2 center = { (float)(width / 2), (float)(height / 2)};
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int index = (x + (y * width)) * channels;
			if (data[index + 3] > 0) {
				V2 point = { (float)x, (float)y };
				V2 direction_vector = center - point;
				result.sprite_radius = hypotenuse(direction_vector.x, direction_vector.y);
				result.sprite_radius = ceil(result.sprite_radius);
			}
		}
	}

	DEFER{
		stbi_image_free(data);
	};

	result.w = width;
	result.h = height;

	result.texture = mp_create_texture(0, 0, result.w, result.h);

	void* pixels;
	int pitch;
	mp_lock_texture(result.texture, NULL, &pixels, &pitch);

	my_mem_copy(data, pixels, ((width * height) * channels));

	mp_unlock_texture(result.texture);
	
	return result;
}

Font load_font(const char* file_path) {
	Font result = {};

	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);

	DEFER{
		stbi_image_free(data);
	};

	result.image.w = width;
	result.image.h = height;

	int index = 0;
	for (int y = 0; y < result.image.h; y++) {
		for (int x = 0; x < result.image.w; x++) {
			index = (4 * ((y * width) + x));
			// Check if the color is black
			if (data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 0) {
				// Set alpha to 0 (Transparent)
				data[index + 3] = 0;
			}
		}
	}

	// Hard coded values (Potentially bad)
	// This function is for bitmap fonts so 
	// this could be okay.
	result.char_width = result.image.w / 18;
	result.char_height = result.image.h / 7;

	MP_Texture* temp = mp_create_texture(0, 0, width, height);

	mp_set_texture_blend_mode(temp, MP_BLENDMODE_BLEND);

	result.image.texture = temp;
	void* pixels;
	int pitch;
	mp_lock_texture(temp, NULL, &pixels, &pitch);

	// Copy what's in my data into the pixels
	// my_Memory_Copy();
	// memcpy(pixels, data, (width * height) * 4);
	my_mem_copy(data, pixels, (width * height) * 4);

	mp_unlock_texture(temp);

	result.image.texture->pitch = pitch;
	result.image.texture->pixels = pixels;

	result.bitmap_width = 18;
	result.bitmap_height = 7;

	result.char_width = result.image.w / result.bitmap_width;
	result.char_height = result.image.h / result.bitmap_height;

	return result;
}

Image dummy_image = {};
std::unordered_map<std::string, Image> images;
void load_images() {
	// Init the dummy image
	dummy_image = load_image("assets\\dummy_image.png");
	images["dummy_image"] = load_image("assets\\dummy_image.png");
	assert(&dummy_image != NULL);

	images["IT_Sun"] = load_image("assets\\sun.png");
	images["IT_Dummy_Player"] = load_image("assets\\dummy_player.png");
	images["IT_Player_Rugged_Male"] = load_image("assets\\player_rugged_male.png");
	images["zombie_male"] = load_image("assets\\zombie_male.png");
	images["IT_Top_Down_Player_Demo"] = load_image("assets\\top_down_player_demo.png");
	images["IT_Arrow_1"] = load_image("assets\\arrow_1.png");
	images["IT_Dummy_Tile_32x32"] = load_image("assets\\dummy_tile_32x32.png");
	images["IT_Rock_32x32"] = load_image("assets\\rock_32x32.png");
	images["IT_Grass_32x32"] = load_image("assets\\grass_32x32.png");
	images["IT_Water_32x32"] = load_image("assets\\water_32x32.png");
	images["temp_zombie_walk"] = load_image("assets\\temp_zombie_walk.png");
}

Image* get_image(std::string image_name) {
	auto iterator = images.find(image_name);

	if (iterator != images.end()) {
		return &iterator->second;
	} else {
		// Return the dummy image so it doesn't crash the game
		return &dummy_image;
	}
}

Font dummy_font = {};
std::unordered_map<Font_Type, Font> fonts;
void load_fonts() {
	dummy_font = load_font("assets\\dummy_font.png");
	fonts[FT_Basic]  = load_font("assets\\basic_font.png");
}

Font* get_font(Font_Type type) {
	auto iterator = fonts.find(type);

	if (iterator != fonts.end()) {
		return &iterator->second;
	} else {
		return &dummy_font;
	}
}

// NOTE: It's important to note here that we are currently drawing outside of the UV coordinates.
// However, setting TexParam to GL_Repeat instead of GL_Mirrored, fixes the issue
void draw_character(Font& font, char character, int x, int y, int size) {
	int ascii = (int)character - (int)' ';
	int src_x = font.char_width * (ascii % font.bitmap_width);
	int src_y = font.char_height * ((font.bitmap_height - 1) - (ascii / font.bitmap_width)) + 1;

	MP_Rect src = { src_x, src_y, font.char_width, font.char_height };
	MP_Rect dst = { x, y, (font.char_width * size), (font.char_height * size) };

	mp_render_copy(font.image.texture, &src, &dst);
}

// Draws in camera space 
void draw_string(Font& font, const char* str, Color_Type c, bool background, int x, int y, int size, bool center_x) {
	int length = (int)strlen(str);

	int final_x = x;
	int final_y = y;

	if (center_x) {
		final_x -= (int)(((float)length / 2.0f) * (float)(font.char_width * size));
		final_y -= (int)(((float)font.char_height * (float)size) / 2.0f);
	}

	if (background) {
		int padding = 3;
		MP_Rect rect = {};
		rect.x = final_x - padding;
		rect.y = final_y - padding;
		rect.w = (length * (font.char_width * size)) + (padding * 2);
		rect.h = (font.char_height * size) + (padding * 2);
		mp_set_render_draw_color(0, 0, 0, 255);
		mp_render_fill_rect(&rect);
	}

	mp_set_texture_color_mod(font.image.texture, c);
	for (int i = 0; i < length; i++) {
		draw_character(font, str[i], final_x + ((font.char_width * size) * i), final_y, size);
	}
	mp_set_texture_color_mod(font.image.texture, 255, 255, 255);
}

void draw_string(Font& font, const char* str, Color_Type c, bool background, float x, float y, int size, bool center_x) {
	draw_string(font, str, c, background, (int)x, (int)y, size, center_x);
}

// Draws in camera space 
void draw_quick_string(Color_Type c, bool background, const char* str, int x, int y) {
	Font* font = get_font(FT_Basic);
	draw_string(*font, str, c, background, x, y, 1, true);
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

