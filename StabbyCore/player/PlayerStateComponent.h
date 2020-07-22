#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "../network/NetworkTypes.h"
#include <cstdint>
#include <cstring>
#include "CharBuffer.h"

enum class State : uint8_t {
	free,
	attacking,
	rolling,
	stunned,
	dead,
	climbing,
	healing,
	crouching
};

struct PlayerState {
	enum StateBits : uint8_t {
		b_game_time = 0,
		b_client_time = 1,
		b_state = 2,
		b_roll_frame = 3,
		b_active_attack = 4,
		b_attack_frame = 5,
		b_health = 6,
		b_stun_frame = 7,
		b_heal_frame = 8,
		b_heal_delay = 9,
		b_facing = 10,
		b_spawn_point = 11,
		b_attack_freeze_frame = 12,
		b_team_id = 13,
		b_stamina = 14,
		b_stamina_recharge_frame = 15,
		b_death_frame = 16,
		b_attack_speed = 17,
		b_move_speed = 18,
		b_weapon_tag = 19,
		b_pos = 20,
		b_vel = 21,
		b_frozen = 22
	};

	bool operator==(const PlayerState & other) {
		return clientTime == other.clientTime && gameTime == other.gameTime && pos == other.pos && vel == other.vel && state == other.state &&
			rollFrame == other.rollFrame && activeAttack == other.activeAttack && attackFrame == other.attackFrame &&
			health == other.health && stunFrame == other.stunFrame && facing == other.facing &&
			spawnPoint == other.spawnPoint && attackFreezeFrame == other.attackFreezeFrame && frozen == other.frozen &&
			attackSpeed == other.attackSpeed && moveSpeed == other.moveSpeed && healFrame == other.healFrame && healDelay == other.healDelay && teamId == other.teamId &&
			stamina == other.stamina && staminaRechargeFrame == other.staminaRechargeFrame && deathFrame == other.deathFrame && weaponTag == other.weaponTag;
	}


	bool operator!=(const PlayerState & other) {
		return !(*this == other);
	}

	Time_t gameTime;
	Time_t clientTime;

	State state;
	uint32_t rollFrame;
	uint32_t activeAttack;
	uint32_t attackFrame;
	int32_t health;
	uint32_t stunFrame;
	uint32_t healFrame;
	uint32_t healDelay;
	int32_t facing;
	Vec2f spawnPoint;
	int32_t attackFreezeFrame;
	uint32_t teamId;
	uint32_t stamina;
	uint32_t staminaRechargeFrame;
	uint32_t deathFrame;
	double attackSpeed;
	double moveSpeed;
	CharBuffer<10> weaponTag;

	Vec2f pos;
	Vec2f vel;
	bool frozen;
};

class PlayerStateComponent {
	//only player controllers should have control over the state. Do not use anything else, it will muddy the design.

public:
	PlayerStateComponent(EntityId id = 0);
	EntityId getId() const;

	PlayerState playerState;
	std::string attackId;
private:
	EntityId id;
};