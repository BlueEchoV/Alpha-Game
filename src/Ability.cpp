#include "Ability.h"

std::unordered_map<std::string, Ability_Data> ability_data_map;

Ability_Data bad_ability_data = {};

Ability_Data get_ability_data(std::string ability_name) {
	auto it = ability_data_map.find(std::string(ability_name));
	if (it == ability_data_map.end()) {
		return bad_ability_data;
	}
	return it->second;
}

Type_Descriptor ability_data_type_descriptors[] = {
	FIELD(Ability_Data, VT_String, ability_name),
	FIELD(Ability_Data, VT_String, ability_type),

	// Grim Arbelist
	FIELD(Ability_Data, VT_Float, damage_increase),
	FIELD(Ability_Data, VT_Float, projectile_speed_increase),
	FIELD(Ability_Data, VT_Float, fire_rate_increase),
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