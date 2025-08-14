#pragma once

#include "Utility.h"

struct Ability_Data {
	std::string ability_name;
	std::string ability_type;

	// Blood Knight

	// Grim Arbelist
	float damage_increase;
	float projectile_speed_increase;
	float fire_rate_increase;
	int projectiles_per_sec;
	float ability_duration_sec;
};

void load_ability_data_csv(CSV_Data* data);

