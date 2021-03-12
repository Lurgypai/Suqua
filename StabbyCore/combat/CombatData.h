#pragma once

/*
	int health;
	uint32_t stamina;
	uint32_t staminaRechargeFrame;
	uint32_t freezeFrame;
	unsigned int teamId;
	unsigned int stunFrame;
	bool invulnerable;
*/

enum CombatData {
	HEALTH = 101,					//int32
	STAMINA = 102,					//uint32
	STAMINA_RECHARGE_FRAME = 103,	//uint32
	FREEZE_FRAME = 104,				//uint32
	TEAM_ID = 105,					//uint32
	STUN_FRAME = 106,				//uint32
	INVULNERABLE = 107				//bool
};