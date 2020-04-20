#include "StatePacket.h"

void StatePacket::serialize() {
	id = htonll(id);
	when = htonll(when);
	state.gameTime = htonll(state.gameTime);
	state.clientTime = htonll(state.clientTime);
	state.rollFrame = htonl(state.rollFrame);
	state.activeAttack = htonl(state.activeAttack);
	state.attackFrame = htonl(state.attackFrame);
	state.health = htonl(state.health);
	state.stunFrame = htonl(state.stunFrame);
	state.facing = htonl(state.facing);
	state.attackFreezeFrame = htonl(state.attackFreezeFrame);
	state.healFrame = htonl(state.healFrame);
	state.healDelay = htonl(state.healDelay);
	state.stamina = htonl(state.stamina);
	state.staminaRechargeFrame = htonl(state.staminaRechargeFrame);
	state.deathFrame = htonl(state.deathFrame);

	pos.x = htonf(state.pos.x);
	pos.y = htonf(state.pos.y);
	vel.x = htonf(state.vel.x);
	vel.y = htonf(state.vel.y);
	spawnPoint.x = htonf(state.spawnPoint.x);
	spawnPoint.y = htonf(state.spawnPoint.y);

	attackSpeed = htond(state.attackSpeed);
	moveSpeed = htond(state.moveSpeed);
}

void StatePacket::unserialize() {
	id = ntohll(id);
	when = ntohll(when);
	state.gameTime = ntohll(state.gameTime);
	state.clientTime = ntohll(state.clientTime);
	state.rollFrame = ntohl(state.rollFrame);
	state.activeAttack = ntohl(state.activeAttack);
	state.attackFrame = ntohl(state.attackFrame);
	state.health = ntohl(state.health);
	state.stunFrame = ntohl(state.stunFrame);
	state.facing = ntohl(state.facing);
	state.attackFreezeFrame = ntohl(state.attackFreezeFrame);
	state.healFrame = ntohl(state.healFrame);
	state.healDelay = ntohl(state.healDelay);
	state.stamina = ntohl(state.stamina);
	state.staminaRechargeFrame = ntohl(state.staminaRechargeFrame);
	state.deathFrame = ntohl(state.deathFrame);

	state.pos.x = ntohf(pos.x);
	state.pos.y = ntohf(pos.y);
	state.vel.x = ntohf(vel.x);
	state.vel.y = ntohf(vel.y);
	state.spawnPoint.x = ntohf(spawnPoint.x);
	state.spawnPoint.y = ntohf(spawnPoint.y);

	state.attackSpeed = ntohd(attackSpeed);
	state.moveSpeed = ntohd(moveSpeed);
}
