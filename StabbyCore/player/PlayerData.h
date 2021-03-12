#pragma once

enum PlayerData {
	STATE = 201,				//char
	ROLL_FRAME = 202,			//uint32_t
	ACTIVE_ATTACK = 203,		//uint32_t
	ATTACK_FRAME = 204,			//uint32_t
	HEAL_FRAME = 205,			//uint32_t
	HEAL_DELAY = 206,			//uint32_t
	ATTACK_FREEZE_FRAME = 207,	//int32_t
	DEATH_FRAME = 208,			//uint32_t
	ATTACK_SPEED = 209,			//double
	MOVE_SPEED = 210,			//double
	WEAPON_TAG = 211,			//string
	USER_TAG = 212				//string
};