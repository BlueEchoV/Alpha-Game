#pragma once

#include "Utility.h"
#include "Player.h"

enum Ability_Type {
	AT_Not_Specified,
	AT_Passive,
	AT_Basic,
	AT_Ultimate
};

struct Ability_Data {
	std::string ability_name;
	std::string ability_type;

	float cooldown;
	float damage_increase;
	float fire_rate_increase;
	float damage_per_stack;
	int projectiles_per_sec;
	float ability_duration_sec;
};

void load_ability_data_csv(CSV_Data* data);

struct Ability {
	std::string name;
	Ability_Type type;

	Player* player;

	// Destructor
	// virtual ~Ability();
	virtual void update_ability(float delta_time) = 0;
	virtual void activate_ability() = 0;
	virtual void draw_ui(V2 camera_pos) = 0;
	// virtual void draw_ability();
};

Ability* equip_ability(Player* player, const std::string& ability_name);
void equip_abilities(Player* player, const std::string& passive, const std::string& basic, const std::string& ultimate);
void free_ability_memory(Ability* ability);

// Portable Ballista: Standing still for 1 s grants +25 % bolt speed & +15 % dmg until you move.
struct Portable_Ballista : Ability {
	float damage_increase = 0.0f;
	// Excess bullets are stored
	float fire_rate_increase = 0.0f;

	float stand_timer = 0.0f;
	bool buff_active = false;
	float damage_buff_value = 0.0f; // float attack_speed_buff_value;

	void update_ability(float delta_time) override;
	void activate_ability() override;
	void draw_ui(V2 camera_pos) override;
};

// Bold Saturation: Overclocks crossbow for 3 s: Fires a backup round at enemies for each projectile shot. (Maplestory rogues with shurakins)
// Bolt Saturation: Consume Ammunation to fire an empowered bolt. The more ammunition consumed, the more powerful the bolt. 
//		Augment Idea: If more than 10 ammunition is consumed, the bolt does something really good
// Hemorrhage Burst: Consume all ammo to fire a piercing bolt that applies a heavy bleed (stacking). Maybe make it so it doesn't consume ammunition
// NOTE: Combinable with the 
struct Hemorrhage_Burst : Ability {
	float damage_per_stack = 0;

	void update_ability(float delta_time) override;
	void activate_ability() override;
	void draw_ui(V2 camera_pos) override;
};

struct Storm_Of_Quarrels: Ability {
	int projectiles_per_sec = 0;
	float ability_duration_sec = 0;

	void update_ability(float delta_time) override;
	void activate_ability() override;
	void draw_ui(V2 camera_pos) override;
};
