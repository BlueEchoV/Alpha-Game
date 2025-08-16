#pragma once

#include "Utility.h"
#include "Player.h"

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

struct Ability {
	std::string name;
	std::string type;

	Player* player;

	// Blood Knight

	// Grim Arbelist
	float damage_increase;
	float projectile_speed_increase;
	float fire_rate_increase;
	int projectiles_per_sec;
	float ability_duration_sec;

	// Destructor
	// virtual ~Ability();
	virtual void update_ability(float delta_time) = 0;
	virtual void activate_ability() = 0;
	virtual void draw_ui(V2 camera_pos) = 0;
	// virtual void draw_ability();
};

Ability* equip_ability(Player* player, const std::string& ability_name);
void equip_abilities(Player* player, const std::string& passive, const std::string& basic, const std::string& ultimate);
void draw_ability_icon(Ability* ability);
void free_ability_memory(Ability* ability);

struct Portable_Ballista : Ability {
	float stand_timer = 0.0f;
	bool buff_active = false;

	float damage_buff_value = 0.0f;
	// float attack_speed_buff_value;

	void update_ability(float delta_time) override;
	void activate_ability() override;
	void draw_ui(V2 camera_pos) override;
};

#if 0
struct Bolt_Saturation : Ability {
	void update_ability(Weapon* weapon) override;
	void activate_ability() override;
};

struct Storm_Of_Quarrels: Ability {
	void update_ability(Weapon* weapon) override;
	void activate_ability() override;
};
#endif
