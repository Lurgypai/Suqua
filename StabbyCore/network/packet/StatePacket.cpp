#include "StatePacket.h"
#include "ByteOrder.h"

#include "DebugFIO.h"

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
	state.teamId = s_htonl(state.teamId);

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
	state.teamId = s_ntohl(state.teamId);

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
	m << s_htonll(id);
	m << s_htonll(when);
	m << controllerState;

	if (state.gameTime == prevState.gameTime)
		m.addEmptyField();
	else
		m << s_htonll(state.gameTime);

	if (state.clientTime == prevState.clientTime)
		m.addEmptyField();
	else
		m << s_htonll(state.clientTime);

	if (state.state == prevState.state)
		m.addEmptyField();
	else
		m << state.state;

	if (state.rollFrame == prevState.rollFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.rollFrame);

	if (state.activeAttack == prevState.activeAttack)
		m.addEmptyField();
	else
		m << s_htonl(state.activeAttack);

	if (state.attackFrame == prevState.attackFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.attackFrame);

	if (state.health == prevState.health)
		m.addEmptyField();
	else
		m << s_htonl(state.health);

	if (state.stunFrame == prevState.stunFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.stunFrame);

	if (state.healFrame == prevState.healFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.healFrame);

	if (state.healDelay == prevState.healDelay)
		m.addEmptyField();
	else
		m << s_htonl(state.healDelay);

	if (state.facing == prevState.facing)
		m.addEmptyField();
	else
		m << s_htonl(state.facing);

	if (state.spawnPoint == prevState.spawnPoint)
		m.addEmptyField();
	else {
		spawnPoint.x = s_htonf(state.spawnPoint.x);
		spawnPoint.y = s_htonf(state.spawnPoint.y);
		m << spawnPoint;
	}

	if (state.attackFreezeFrame == prevState.attackFreezeFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.attackFreezeFrame);

	if (state.teamId == prevState.teamId)
		m.addEmptyField();
	else
		m << s_htonl(state.teamId);

	if (state.stamina == prevState.stamina)
		m.addEmptyField();
	else
		m << s_htonl(state.stamina);

	if (state.staminaRechargeFrame == prevState.staminaRechargeFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.staminaRechargeFrame);

	if (state.deathFrame == prevState.deathFrame)
		m.addEmptyField();
	else
		m << s_htonl(state.deathFrame);

	if (state.attackSpeed == prevState.attackSpeed)
		m.addEmptyField();
	else {
		attackSpeed = s_htond(state.attackSpeed);
		m << attackSpeed;
	}

	if (state.moveSpeed == prevState.moveSpeed)
		m.addEmptyField();
	else {
		moveSpeed = s_htond(state.moveSpeed);
		m << moveSpeed;
	}

	if (state.weaponTag == prevState.weaponTag)
		m.addEmptyField();
	else
		m << state.weaponTag;

	if (state.pos == prevState.pos)
		m.addEmptyField();
	else {
		pos.x = s_htonf(state.pos.x);
		pos.y = s_htonf(state.pos.y);
		m << pos;
		DebugFIO::Out("s_out.txt") << "sent pos " << state.pos << '\n';
	}

	if (state.vel == prevState.vel)
		m.addEmptyField();
	else {
		vel.x = s_htonf(state.vel.x);
		vel.y = s_htonf(state.vel.y);
		m << vel;
		DebugFIO::Out("s_out.txt") << "sent vel " << state.vel << '\n';
	}

	if (state.frozen == prevState.frozen)
		m.addEmptyField();
	else
		m << state.frozen;

	DebugFIO::Out("s_out.txt") << " partial update, pos: " << state.pos << " vel: " << state.vel << " clientTime: " << state.clientTime << '\n';
}

void StatePacket::readInto(MarkedStream& m) {
	serialize();

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

	DebugFIO::Out("s_out.txt") << "full update, pos: " << state.pos << " vel: " << state.vel << " clientTime: " << state.clientTime << '\n';
}

DynamicBitset StatePacket::readFrom(MarkedStream& m) {
	m >> id;
	id = s_ntohll(id);
	m >> when;
	when = s_ntohll(when);
	m >> controllerState;

	DynamicBitset b;
	b.resize(23); // number of fields in PlayerState;

	if (m >> state.gameTime) {
		state.gameTime = s_ntohll(state.gameTime);
		b.set(0, true);
	}
	if (m >> state.clientTime) {
		state.clientTime = s_ntohll(state.clientTime);
		b.set(1, true);
	}
	if (m >> state.state) {
		b.set(2, true);
	}

	if (m >> state.rollFrame) {
		state.rollFrame = s_ntohl(state.rollFrame);
		b.set(3, true);
	}
	if (m >> state.activeAttack) {
		state.activeAttack = s_ntohl(state.activeAttack);
		b.set(4, true);
	}
	if (m >> state.attackFrame) {
		state.attackFrame = s_ntohl(state.attackFrame);
		b.set(5, true);
	}
	if (m >> state.health) {
		state.health = s_ntohl(state.health);
		b.set(6, true);
	}
	if (m >> state.stunFrame) {
		state.stunFrame = s_ntohl(state.stunFrame);
		b.set(7, true);
	}
	if (m >> state.healFrame) {
		state.healFrame = s_ntohl(state.healFrame);
		b.set(8, true);
	}
	if (m >> state.healDelay) {
		state.healDelay = s_ntohl(state.healDelay);
		b.set(9, true);
	}
	if (m >> state.facing) {
		state.facing = s_ntohl(state.facing);
		b.set(10, true);
	}
	if (m >> spawnPoint) {
		state.spawnPoint.x = s_ntohf(spawnPoint.x);
		state.spawnPoint.y = s_ntohf(spawnPoint.y);
		b.set(11, true);
	}
	if (m >> state.attackFreezeFrame) {
		state.attackFreezeFrame = s_ntohl(state.attackFreezeFrame);
		b.set(12, true);
	}
	if (m >> state.teamId) {
		state.teamId = s_ntohl(state.teamId);
		b.set(13, true);
	}
	if (m >> state.stamina) {
		state.stamina = s_ntohl(state.stamina);
		b.set(14, true);
	}
	if (m >> state.staminaRechargeFrame) {
		state.staminaRechargeFrame = s_ntohl(state.staminaRechargeFrame);
		b.set(15, true);
	}
	if (m >> state.deathFrame) {
		state.deathFrame = s_ntohl(state.deathFrame);
		b.set(16, true);
	}
	if (m >> moveSpeed) {
		state.moveSpeed = s_ntohd(moveSpeed);
		b.set(17, true);
	}
	if (m >> attackSpeed) {
		state.attackSpeed = s_ntohd(attackSpeed);
		b.set(18, true);
	}

	if (m >> state.weaponTag) {
		b.set(19, true);
	}

	if (m >> pos) {
		state.pos.x = s_ntohf(pos.x);
		state.pos.y = s_ntohf(pos.y);
		b.set(20, true);
	}
	if (m >> vel) {
		state.vel.x = s_ntohf(vel.x);
		state.vel.y = s_ntohf(vel.y);
		b.set(21, true);
	}

	if (m >> state.frozen) {
		b.set(22, true);
	}
	return b;
}
