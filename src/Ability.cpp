#include "Ability.h"

std::unordered_map<std::string, Ability_Data> ability_data_map;

Ability_Data bad_ability_data = {};

Ability_Data get_ability_data(const std::string& ability_name) {
	auto it = ability_data_map.find(std::string(ability_name));
	if (it == ability_data_map.end()) {
		return bad_ability_data;
	}
	return it->second;
}

void draw_ability_icon(Ability_Type type, const std::string& ability_name) {
	std::string image_name = ability_name;
	V2 icon_pos = {};
	icon_pos.x = ((float)Globals::playground_area_w / 2.0f) - (Globals::ability_icon_w / 2.0f);
	icon_pos.y = Globals::ability_icon_w / 2;

	switch (type) {
	case AT_Passive: {
		icon_pos.x -= (float)Globals::ability_icon_w * 1.25;
		image_name += "_passive_icon";
		break;
	}
	case AT_Basic: {
		// Positioned in the center
		image_name += "_basic_icon";
		break;
	}
	case AT_Ultimate: {
		icon_pos.x += (float)Globals::ability_icon_w * 1.25;
		image_name += "_ultimate_icon";
		break;
	}
	default: {
		log("Error: Ability_Type not specified");
		break;
	}
	}

	MP_Rect dst_rect = {(int)icon_pos.x, (int)icon_pos.y, Globals::ability_icon_w, Globals::ability_icon_w};

	// Image* image = get_image(image_name);
	Sprite_Sheet* ss = get_sprite_sheet(image_name);
	Image* image = &ss->sprites[0].image;
	if (image != nullptr) {
		mp_render_copy_ex(image->texture, NULL, &dst_rect, 0, NULL, SDL_FLIP_NONE);
	}
}

// Portable Ballista: Standing still for 1 s grants +25 % bolt speed & +15 % dmg until you move.
void Portable_Ballista::update_ability(float delta_time) {
	// NOTE: Floating-point numbers aren't perfectly precise because they have limited bits to store very large or 
	// very small numbers. This causes rounding errors, especially with decimals that can't be exactly 
	// represented in binary (like 0.1).
	bool is_still = player->rb.vel.x <= 0.001f && player->rb.vel.y <= 0.001f; 

	bool reset_buffs = false;

	if (is_still) {
		this->stand_timer += delta_time;
	}
	else {
		if (this->buff_active == true) {
			this->stand_timer = 0.0f;
			reset_buffs = true;
		}
	}

	if (reset_buffs) {
		player->weapon->damage -= (int)this->damage_buff_value;
		this->buff_active = false;
		return;
	}
	if (this->stand_timer >= 1.0f && this->buff_active == false) {
		this->damage_buff_value = player->weapon->base_damage * this->damage_increase;
		player->weapon->damage += (int)this->damage_buff_value;
		this->buff_active = true;
	}
}

void Portable_Ballista::activate_ability() {
	// Passive so do nothing
}

void Portable_Ballista::draw_ui(V2 camera_pos) {
	if (this->buff_active) {
		// TODO: Pass this in somehow?
		Font* font = get_font(FT_Basic);

		V2 pos_cs = convert_ws_to_cs(player->rb.pos_ws, camera_pos);
		draw_string(*font, "+15% damage", CT_Dark_Yellow, true, pos_cs.x, pos_cs.y + player->h / 2 + player->health_bar.h * 3, 1, true);
	}
	draw_ability_icon(AT_Passive, this->name);
}

void Hemorrhage_Burst::update_ability(float delta_time) {
	REF(delta_time);
	draw_ability_icon(this->type, this->name);
}

void Hemorrhage_Burst::activate_ability() {
	Ability_Data data = get_ability_data(this->name);

	this->damage_per_stack = data.damage_per_stack;
}

void Hemorrhage_Burst::draw_ui(V2 camera_pos) {
	REF(camera_pos);
	draw_ability_icon(this->type, this->name);
}

void Storm_Of_Quarrels::update_ability(float delta_time) {
	REF(delta_time);

}

void Storm_Of_Quarrels::activate_ability() {

}

void Storm_Of_Quarrels::draw_ui(V2 camera_pos) {
	REF(camera_pos);
	draw_ability_icon(this->type, this->name);
}


Ability* equip_ability(Player* player, const std::string& ability_name) {
	Ability_Data ability_data = get_ability_data(ability_name);

	Ability* ability = nullptr;
	if (ability_name == "portable_ballista") {
		Portable_Ballista* pb = new Portable_Ballista();
		pb->damage_increase = ability_data.damage_increase;
		pb->fire_rate_increase = ability_data.fire_rate_increase;
		ability = pb;

	} else if (ability_name == "hemorrhage_burst") {
		Hemorrhage_Burst* hb = new Hemorrhage_Burst();
		hb->damage_per_stack = ability_data.damage_per_stack;
		ability = hb;

	} else if (ability_name == "storm_of_quarrels") {
		Storm_Of_Quarrels* soq = new Storm_Of_Quarrels();
		soq->projectiles_per_sec = ability_data.projectiles_per_sec;
		soq->ability_duration_sec = ability_data.ability_duration_sec;
		ability = soq;
	}
	else {
		// Handle unknown: return nullptr or throw
		log("create_ability : Ability Name Not Specified");
		return nullptr;
	}

	if (ability_data.ability_type == "passive") {
		ability->type = AT_Passive;
	}
	else if (ability_data.ability_type == "basic") {
		ability->type = AT_Basic;
	}
	else if (ability_data.ability_type == "ultimate") {
		ability->type = AT_Ultimate;
	}
	else {
		log("Error: Ability Type not specified");
		return nullptr;
	}

	ability->player = player;
	ability->name = ability_data.ability_name;

	return ability;
}

void equip_abilities(Player* player, const std::string& passive, const std::string& basic, const std::string& ultimate) {
	player->passive = equip_ability(player, passive);
	player->basic = equip_ability(player, basic);
	player->ultimate = equip_ability(player, ultimate);
}

Type_Descriptor ability_data_type_descriptors[] = {
	FIELD(Ability_Data, VT_String, ability_name),
	FIELD(Ability_Data, VT_String, ability_type),

	// Grim Arbelist
	FIELD(Ability_Data, VT_Float, cooldown),
	FIELD(Ability_Data, VT_Float, damage_increase),
	FIELD(Ability_Data, VT_Float, fire_rate_increase),
	FIELD(Ability_Data, VT_Float, damage_per_stack),
	FIELD(Ability_Data, VT_Int, projectiles_per_sec),
	FIELD(Ability_Data, VT_Float, ability_duration_sec)
};

void load_ability_data_csv(CSV_Data* data) {
	std::vector<Ability_Data> ability_data;

	ability_data.resize(data->total_rows);

	std::span<Type_Descriptor> safe_ability_data_type_descriptors(ability_data_type_descriptors);

	load_csv_data_file(data, (char*)ability_data.data(), safe_ability_data_type_descriptors, sizeof(Ability_Data));

	for (Ability_Data& ability_data_iterator : ability_data) {
		ability_data_map[ability_data_iterator.ability_name] = ability_data_iterator;
	}
}