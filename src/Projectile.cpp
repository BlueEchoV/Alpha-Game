#include "Projectile.h"

std::unordered_map<std::string, Projectile_Data> projectile_data_map;

#if 0
Projectile_Data bad_projectile_data = {
	// name		w	h	speed
	"arrow",	32, 32, 100
};

Projectile_Data get_projectile_data(std::string_view projectile_name) {
	auto it = projectile_data_map.find(std::string(projectile_name));
	if (it == projectile_data_map.end()) {
		return bad_projectile_data;
	}
	return it->second;
}
#endif

void spawn_projectile(std::vector<Handle>& projectile_handles, Storage<Projectile>& projectile_storage,
	std::string_view projectile_name, int damage, int speed, int w, int h, V2 origin_ws, V2 target_ws) {

	Projectile result = {};
	result.damage = damage;

	result.rb = create_rigid_body(origin_ws, speed);
	V2 vel_normalized = calculate_normalized_origin_to_target_velocity(target_ws, origin_ws);
	result.rb.vel = vel_normalized;

	result.at = create_animation_tracker(ATT_Direction_2, projectile_name, AS_No_Animation, APS_Speed_Based, AM_Animate_Looping, true);

	result.w = w;
	result.h = h;
	result.rb.angle = calculate_facing_direction_east_counterclockwise(result.rb.vel);

	Sprite_Sheet* sprite_sheet = get_sprite_sheet(std::string(projectile_name));
	float first_sprite_radius = sprite_sheet->sprites[0].image.sprite_radius;
	V2 pos_ls = rotate_point_based_off_angle(result.rb.angle, 0, 0, first_sprite_radius * 0.8f, 0);

	add_collider(&result.rb, pos_ls, first_sprite_radius / 4);

	result.handle = create_handle(projectile_storage);
	projectile_handles.push_back(result.handle);
	projectile_storage.storage[result.handle.index] = result;
}

void update_projectile(Projectile& projectile, float delta_time) {
	projectile.rb.pos_ws.x += projectile.rb.current_speed * (projectile.rb.vel.x * delta_time);
	projectile.rb.pos_ws.y += projectile.rb.current_speed * (projectile.rb.vel.y * delta_time);
}

void draw_projectile(int camera_pos_x, int camera_pos_y, Projectile& projectile) {
	// Draw everything around the camera (converting to camera space)
	V2 entity_pos_cs = convert_ws_to_cs(projectile.rb.pos_ws, { (float)camera_pos_x, 
		(float)camera_pos_y });
	// Center the image on the position of the entity
	MP_Rect dst = { (int)entity_pos_cs.x - projectile.w / 2, 
		(int)entity_pos_cs.y - projectile.h / 2, projectile.w, projectile.h };

	draw_animation_tracker(&projectile.at, dst, projectile.rb.angle);
}

/*
Type_Descriptor projectile_data_type_descriptors[] = {
	FIELD(Projectile_Data, VT_String, projectile_name),
	FIELD(Projectile_Data, VT_Int, w),
	FIELD(Projectile_Data, VT_Int, h)
};

void load_projectile_data_csv(CSV_Data* data) {
	std::vector<Projectile_Data> projectile_data;

	projectile_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_projectile_data_type_descriptors(projectile_data_type_descriptors);

	load_csv_data_file(data, (char*)projectile_data.data(), safe_projectile_data_type_descriptors, sizeof(Projectile_Data));

	for (Projectile_Data& projectile_data_iterator : projectile_data) {
		projectile_data_map[projectile_data_iterator.projectile_name] = projectile_data_iterator;
	}
}
*/
