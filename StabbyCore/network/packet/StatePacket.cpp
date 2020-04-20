#include "StatePacket.h"
#include "ByteOrder.h"

void StatePacket::serialize() {
	id = s_htonll(id);
	when = s_htonll(when);
	state.gameTime = s_htonll(state.gameTime);
	state.clientTime = s_htonll(state.clientTime);
	state.rollFrame = s_htonl(state.rollFrame);
	state.activeAttack = s_htonl(state.activeAttack);
	state.attackFrame = s_htonl(state.attackFrame);
	state.health = s_htonl(state.health);
	state.stunFrame = s_htonl(state.stunFrame);
	state.facing = s_htonl(state.facing);
	state.attackFreezeFrame = s_htonl(state.attackFreezeFrame);
	state.healFrame = s_htonl(state.healFrame);
	state.healDelay = s_htonl(state.healDelay);
	state.stamina = s_htonl(state.stamina);
	state.staminaRechargeFrame = s_htonl(state.staminaRechargeFrame);
	state.deathFrame = s_htonl(state.deathFrame);

	pos.x = s_htonf(state.pos.x);
	pos.y = s_htonf(state.pos.y);
	vel.x = s_htonf(state.vel.x);
	vel.y = s_htonf(state.vel.y);
	spawnPoint.x = s_htonf(state.spawnPoint.x);
	spawnPoint.y = s_htonf(state.spawnPoint.y);

	attackSpeed = s_htond(state.attackSpeed);
	moveSpeed = s_htond(state.moveSpeed);
}

void StatePacket::unserialize() {
	id = s_ntohll(id);
	when = s_ntohll(when);
	state.gameTime = s_ntohll(state.gameTime);
	state.clientTime = s_ntohll(state.clientTime);
	state.rollFrame = s_ntohl(state.rollFrame);
	state.activeAttack = s_ntohl(state.activeAttack);
	state.attackFrame = s_ntohl(state.attackFrame);
	state.health = s_ntohl(state.health);
	state.stunFrame = s_ntohl(state.stunFrame);
	state.facing = s_ntohl(state.facing);
	state.attackFreezeFrame = s_ntohl(state.attackFreezeFrame);
	state.healFrame = s_ntohl(state.healFrame);
	state.healDelay = s_ntohl(state.healDelay);
	state.stamina = s_ntohl(state.stamina);
	state.staminaRechargeFrame = s_ntohl(state.staminaRechargeFrame);
	state.deathFrame = s_ntohl(state.deathFrame);

	state.pos.x = s_ntohf(pos.x);
	state.pos.y = s_ntohf(pos.y);
	state.vel.x = s_ntohf(vel.x);
	state.vel.y = s_ntohf(vel.y);
	state.spawnPoint.x = s_ntohf(spawnPoint.x);
	state.spawnPoint.y = s_ntohf(spawnPoint.y);

	state.attackSpeed = s_ntohd(attackSpeed);
	state.moveSpeed = s_ntohd(moveSpeed);
}
