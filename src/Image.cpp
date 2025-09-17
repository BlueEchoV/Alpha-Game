#include "Image.h"

// SEE SPRITE_SYSTEM.CPP FOR NOTES ON THIS
Image load_image(const char* file_path, int access, bool use_linear_filtering) {
	Image result = {};

	// Might be necessary for open gl, testing soon
	stbi_set_flip_vertically_on_load(true); 

	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);
	if (data == NULL) {
		log("Error: stbi_load is null: %s", file_path);
		assert(false);
		result = {};
		return result;
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

	result.texture = mp_create_texture(0, access, result.w, result.h, use_linear_filtering);

	void* pixels;
	int pitch;
	mp_lock_texture(result.texture, NULL, &pixels, &pitch);

	my_mem_copy(data, pixels, ((width * height) * channels));

	mp_unlock_texture(result.texture);
	
	return result;
}

Image load_image(const char* file_path) {
	return load_image(file_path, 0, false);
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
	images["idle_zombie_male"] = load_image("assets\\idle_zombie_male.png");
	images["IT_Top_Down_Player_Demo"] = load_image("assets\\top_down_player_demo.png");
	images["IT_Dummy_Tile_32x32"] = load_image("assets\\dummy_tile_32x32.png");
	images["IT_Water_32x32"] = load_image("assets\\water_32x32.png");
	images["temp_zombie_walk"] = load_image("assets\\temp_zombie_walk.png");
	images["idle_temp_zombie"] = load_image("assets\\idle_temp_zombie.png");
	images["tree"] = load_image("assets\\tree.png");
	images["rock"] = load_image("assets\\rock.png");
	images["bush"] = load_image("assets\\bush.png");

	images["IT_Rock_32x32"] = load_image("assets\\rock_32x32.png", 0, true);
	images["IT_Grass_32x32"] = load_image("assets\\grass_32x32.png", 0, true);

	images["IT_Rock_32x32"] = load_image("assets\\rock_32x32.png", 0, true);
	images["IT_Grass_32x32"] = load_image("assets\\grass_32x32.png", 0, true);

	images["cathedral_tile_one"] = load_image("assets\\cathedral_tile_one.png", 0, true);
	images["cathedral_tile_two"] = load_image("assets\\cathedral_tile_two.png", 0, true);
}

Image* get_image(std::string_view image_name) {
	auto iterator = images.find(std::string(image_name));

	if (iterator != images.end()) {
		return &iterator->second;
	} else {
		// Return the dummy image so it doesn't crash the game
		return &dummy_image;
	}
}