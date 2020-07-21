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

void StatePacket::readInto(MarkedStream& m, const PlayerState& prevState) {
	m << id;
	m << when;
	m << controllerState;

	if (state.gameTime == prevState.gameTime)
		m.addEmptyField();
	else
		m << state.gameTime;

	if (state.clientTime == prevState.clientTime)
		m.addEmptyField();
	else
		m << state.clientTime;

	if (state.state == prevState.state)
		m.addEmptyField();
	else
		m << state.state;

	if (state.rollFrame == prevState.rollFrame)
		m.addEmptyField();
	else
		m << state.rollFrame;

	if (state.activeAttack == prevState.activeAttack)
		m.addEmptyField();
	else
		m << state.activeAttack;

	if (state.attackFrame == prevState.attackFrame)
		m.addEmptyField();
	else
		m << state.attackFrame;

	if (state.health == prevState.health)
		m.addEmptyField();
	else
		m << state.health;

	if (state.stunFrame == prevState.stunFrame)
		m.addEmptyField();
	else
		m << state.stunFrame;

	if (state.healFrame == prevState.healFrame)
		m.addEmptyField();
	else
		m << state.healFrame;

	if (state.healDelay == prevState.healDelay)
		m.addEmptyField();
	else
		m << state.healDelay;

	if (state.facing == prevState.facing)
		m.addEmptyField();
	else
		m << state.facing;

	if (state.spawnPoint == prevState.spawnPoint)
		m.addEmptyField();
	else
		m << spawnPoint;

	if (state.attackFreezeFrame == prevState.attackFreezeFrame)
		m.addEmptyField();
	else
		m << state.attackFreezeFrame;

	if (state.teamId == prevState.teamId)
		m.addEmptyField();
	else
		m << state.teamId;

	if (state.stamina == prevState.stamina)
		m.addEmptyField();
	else
		m << state.stamina;

	if (state.staminaRechargeFrame == prevState.staminaRechargeFrame)
		m.addEmptyField();
	else
		m << state.staminaRechargeFrame;

	if (state.deathFrame == prevState.deathFrame)
		m.addEmptyField();
	else
		m << state.deathFrame;

	if (state.attackSpeed == prevState.attackSpeed)
		m.addEmptyField();
	else
		m << attackSpeed;

	if (state.moveSpeed == prevState.moveSpeed)
		m.addEmptyField();
	else
		m << moveSpeed;

	if (state.weaponTag == prevState.weaponTag)
		m.addEmptyField();
	else
		m << state.weaponTag;

	if (state.pos == prevState.pos)
		m.addEmptyField();
	else
		m << pos;

	if (state.vel == prevState.vel)
		m.addEmptyField();
	else
		m << vel;

	if (state.frozen == prevState.frozen)
		m.addEmptyField();
	else
		m << state.frozen;
}

void StatePacket::readInto(MarkedStream& m) {
	m << id;
	m << when;
	m << controllerState;

	m << state.gameTime;
	m << state.clientTime;
	m << state.state;
	m << state.rollFrame;
	m << state.activeAttack;
	m << state.attackFrame;
	m << state.health;
	m << state.stunFrame;
	m << state.healFrame;
	m << state.healDelay;
	m << state.facing;
	m << spawnPoint;
	m << state.attackFreezeFrame;
	m << state.teamId;
	m << state.stamina;
	m << state.staminaRechargeFrame;
	m << state.deathFrame;
	m << attackSpeed;
	m << moveSpeed;
	m << state.weaponTag;
	m << pos;
	m << vel;
	m << state.frozen;
}

void StatePacket::readFrom(MarkedStream& m) {
	m >> id;
	m >> when;
	m >> controllerState;

	m >> state.gameTime;
	m >> state.clientTime;
	m >> state.state;
	m >> state.rollFrame;
	m >> state.activeAttack;
	m >> state.attackFrame;
	m >> state.health;
	m >> state.stunFrame;
	m >> state.healFrame;
	m >> state.healDelay;
	m >> state.facing;
	m >> spawnPoint;
	m >> state.attackFreezeFrame;
	m >> state.teamId;
	m >> state.stamina;
	m >> state.staminaRechargeFrame;
	m >> state.deathFrame;
	m >> attackSpeed;
	m >> moveSpeed;
	m >> state.weaponTag;
	m >> pos;
	m >> vel;
	m >> state.frozen;
}
