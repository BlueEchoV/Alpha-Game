#include "Rigidbody.h"

V2 convert_cs_to_ws(V2 entity_pos, V2 camera_pos) {
	return entity_pos + camera_pos;
}

V2 convert_cs_to_ws(int entity_x, int entity_y, int camera_x, int camera_y) {
	return convert_cs_to_ws({(float)entity_x, (float)entity_y }, {(float)camera_x, (float)camera_y });
}

V2 convert_ws_to_cs(V2 entity_pos, V2 camera_pos) {
	return entity_pos - camera_pos;
}

V2 convert_ws_to_cs(int entity_x, int entity_y, int camera_x, int camera_y) {
	return convert_ws_to_cs({(float)entity_x, (float)entity_y }, {(float)camera_x, (float)camera_y });
}

void draw_circle_ws(Color_Type c, V2 center_pos_ws, int radius, float total_lines) {
	V2 pos_cs = center_pos_ws;

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

		x1 += pos_cs.x;
		y1 += pos_cs.y;
		x2 += pos_cs.x;
		y2 += pos_cs.y;

		mp_render_draw_line((int)x1, (int)y1, (int)x2, (int)y2);
	}
}

void draw_circle_cs(Color_Type c, V2 center_pos_ws, V2 camera_pos, int radius, float total_lines) {
	V2 pos_cs = convert_ws_to_cs(center_pos_ws, camera_pos);

	draw_circle_ws(c, pos_cs, radius, total_lines);
}

void add_collider(Rigid_Body* rb, V2 pos_ls, float radius) {
	if (rb->num_colliders < Globals::MAX_COLLIDERS) {
		Collider collider = {};

		collider.pos_ls = pos_ls;
		collider.radius = radius;

		rb->colliders[rb->num_colliders] = collider;
		rb->num_colliders++; 
	}
}

void draw_colliders(Rigid_Body* rb, V2 camera_pos) {
	for (int i = 0; i < rb->num_colliders; i++) {
		Collider* c = &rb->colliders[i];
		if (c == NULL) {
			continue;
		}

		V2 collider_ws_pos = rb->pos_ws + c->pos_ls;
		draw_circle_cs(CT_Green, collider_ws_pos, camera_pos, (int)c->radius, 20);
	}
}

bool check_rb_collision(Rigid_Body* rb_1, Rigid_Body* rb_2) {
	for (int i = 0; i < rb_1->num_colliders; i++) {
		for (int j = 0; j < rb_2->num_colliders; j++) {
			Collider* collider_a = &rb_1->colliders[i];
			Collider* collider_b = &rb_2->colliders[j];

			V2 collider_a_ws_pos = rb_1->pos_ws + collider_a->pos_ls;
			V2 collider_b_ws_pos = rb_2->pos_ws + collider_b->pos_ls;

			float radius_sum = collider_a->radius + collider_b->radius;
			float distance_between = calculate_distance_between(collider_a_ws_pos, collider_b_ws_pos);

			if (distance_between < radius_sum) {
				return true;
			}
		}
	}

	return false;
}

Rigid_Body create_rigid_body(V2 pos_ws, int speed) {
	Rigid_Body result = {};

	result.pos_ws = pos_ws;
	result.base_speed = speed;
	result.current_speed = result.base_speed;
	result.angle = 0.0f;

	return result;
}