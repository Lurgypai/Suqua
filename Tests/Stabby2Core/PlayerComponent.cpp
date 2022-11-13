#include "PlayerComponent.h"
#include "PhysicsComponent.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"

#include <iostream>
#include <fstream>
#include <filesystem>

std::ofstream PlayerComponent::playerLog{};

using NDC = NetworkDataComponent;
using namespace PlayerData;

PlayerComponent::PlayerComponent(EntityId id_) :
	id{ id_ },
	jumpSquatMax{ 8 },
	landingMax{ 8 },
	jumpVel{ 350 },
	shortHopVel{ 220 },
	moveSpeed{ 80 },
	groundedDecel{ 10 },
	dodgeSpeed{ 150 },
	dodgeMax{ 58 }
{
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || !EntitySystem::GetComp<PhysicsComponent>(id)) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->setRes({ 4, 20 });
			physics->teleport({ 480 / 2, 270 / 2 });
			physics->setWeight(8);
		}
		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}

		NDC* data = EntitySystem::GetComp<NDC>(id);
		data->set<uint32_t>(ACTION_FRAME, 0);
		data->set<int32_t>(DIR, 1);
		data->set<int32_t>(DODGE_DIR, 1);
		data->set<bool>(BUFFER_ATTACK, false);
		data->set<uint32_t>(STATE, PlayerState::idle);
		data->set<uint32_t>(FREEZE_STATE, PlayerState::idle);
		data->set<uint32_t>(CURR_ATTACK, PlayerAttack::none);
		data->set<uint32_t>(PREV_ATTACK, PlayerAttack::none);
		data->set<uint32_t>(CURR_HITBOX, 0);
		data->set<uint32_t>(PREV_HITBOX, 0);
		data->set<uint32_t>(ATTACK_ELAPSED_TIME, 0);
		data->set<uint32_t>(PERCENT, 0);
		data->set<uint32_t>(FREEZE_FRAME, 0);
	}

	attacks.resize(4);
	attacks[PlayerAttack::slash1].addHitbox({ AABB{ {0, -21}, {16, 13} } }, 300, 25);
	attacks[PlayerAttack::slash2].addHitbox({ AABB{ {4, -23}, {16, 22} } }, 350, 35);
	attacks[PlayerAttack::slash3].addHitbox({}, 100, 0);
	attacks[PlayerAttack::slash3].addHitbox({ AABB{ {8, -17, }, { 19, 17 } }, AABB{ {-3, -24}, {25, 11} } }, 300, 40);
}

EntityId PlayerComponent::getId() const {
	return id;
}

void PlayerComponent::update(const Game& game) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	// I don't like this solution, as it means intermixing the netcode in here, which I disapprove of. oh well.
	// Set the currHitboxIndex if it was updated by the netcode
	uint32_t currAttack = data->get<uint32_t>(CURR_ATTACK);
	if (currAttack != PlayerAttack::none) {
		attacks[currAttack].setCurrHitboxIndex(data->get<uint32_t>(CURR_HITBOX));
		attacks[currAttack].setElapsedTime(data->get<uint32_t>(ATTACK_ELAPSED_TIME));

		//also store the previous
		data->get<uint32_t>(PREV_ATTACK) = data->get<uint32_t>(CURR_ATTACK);
		data->get<uint32_t>(PREV_HITBOX) = data->get<uint32_t>(CURR_HITBOX);
	}

	switch (data->get<uint32_t>(STATE)) {
	case PlayerState::idle: _idle(); break;
	case PlayerState::walking: _walking(); break;
	case PlayerState::dodge: _dodge(); break;
	case PlayerState::grounded_attack: _groundedAttack(game); break;
	case PlayerState::respawning: _respawning(game); break;
	case PlayerState::hitstun: _hitstun(); break;
	case PlayerState::freeze: _freeze(); break;
	}

	//set the synchronized currHitboxIndex
	if (currAttack != PlayerAttack::none) {
		data->get<uint32_t>(CURR_HITBOX) = attacks[currAttack].getCurrHitboxIndex();
		data->get<uint32_t>(ATTACK_ELAPSED_TIME) = attacks[currAttack].getElapsedTime();
	}

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	if (physics->getPos().y > 320) {
		beginRespawn();
	}

	//// DEBUG LOG
	//if (!playerLog.good()) {
	//	playerLog.open("player_log.csv");
	//	playerLog << "ID,Tick,Input,Attack,AttackFrame\n";
	//	std::cout << "Begain logging for player " << id << '\n';
	//	std::cout << "Current directory: " << std::filesystem::current_path() << '\n';
	//}

	//auto contComp = EntitySystem::GetComp<ControllerComponent>(id);
	//auto c = contComp->getController();

	//playerLog << id << ',' << game.getGameTick() << ',' << c.getState() << ',' << data->get<uint32_t>(CURR_ATTACK) << ',' << data->get<uint32_t>(CURR_HITBOX) << '\n';
}

PlayerComponent::PlayerState PlayerComponent::getState() const
{
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return static_cast<PlayerState>(data->get<uint32_t>(STATE));
}

PlayerComponent::PlayerState PlayerComponent::getFrozenState() const
{
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return static_cast<PlayerState>(data->get<uint32_t>(FREEZE_STATE));
}

void PlayerComponent::doRespawn() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	physics->teleport({ 480 / 2, 270 / 2 });

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(PERCENT) = 0;
}

void PlayerComponent::damage(int damage_) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(PERCENT) += damage_;

	beginHitstun();
}

void PlayerComponent::markHit(EntityId otherPlayer) {
	hitEntities.emplace(otherPlayer);
}

bool PlayerComponent::hasHit(EntityId otherPlayer) {
	return hitEntities.find(otherPlayer) != hitEntities.end();
}

bool PlayerComponent::hitboxChanged() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<uint32_t>(CURR_HITBOX) != data->get<uint32_t>(PREV_HITBOX) ||
		data->get<uint32_t>(CURR_ATTACK) != data->get<uint32_t>(PREV_ATTACK);
}

void PlayerComponent::clearMarked() {
	hitEntities.clear();
}

std::optional<Attack> PlayerComponent::getCurrAttack() const {
	PlayerAttack currAttack = static_cast<PlayerAttack>(EntitySystem::GetComp<NDC>(id)->get<uint32_t>(CURR_ATTACK));
	if (currAttack != PlayerAttack::none)
		return std::optional<Attack>{attacks[currAttack]};
	return std::nullopt;
}

PlayerComponent::PlayerAttack PlayerComponent::getCurrAttackId() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return static_cast<PlayerAttack>(data->get<uint32_t>(CURR_ATTACK));
}

void PlayerComponent::_idle() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	int inputDir = 0;
	if (c[ControllerBits::LEFT]) --inputDir;
	if (c[ControllerBits::RIGHT]) ++inputDir;

	if (inputDir != 0) {
		data->get<uint32_t>(STATE) = PlayerState::walking;
		return;
	}
	if (c.toggled(ControllerBits::BUTTON_11) && c[ControllerBits::BUTTON_11]) {
		beginGroundedAttack();
		return;
	}

	decelerate();
}

void PlayerComponent::_walking() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	int inputDir = 0;
	if (c[ControllerBits::LEFT]) --inputDir;
	if (c[ControllerBits::RIGHT]) ++inputDir;

	if (inputDir != 0) data->get<int32_t>(DIR) = inputDir;

	if (c[ControllerBits::BUTTON_6]) {
		beginDodge();
		return;
	}
	if (c.toggled(ControllerBits::BUTTON_11) && c[ControllerBits::BUTTON_11]) {
		beginGroundedAttack();
		return;
	}

	auto vel = physics->getVel();
	vel.x = inputDir * moveSpeed;

	if (vel.x == 0) {
		data->get<uint32_t>(STATE) = PlayerState::idle;
		return;
	}
	else {
		physics->setVel(vel);
	}
}

inline void PlayerComponent::beginDodge() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(ACTION_FRAME) = 0;
	data->get<int32_t>(DODGE_DIR) = data->get<int32_t>(DIR);
	data->get<uint32_t>(STATE) = PlayerState::dodge;
}

void PlayerComponent::_dodge() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	uint32_t& actionFrame = data->get<uint32_t>(ACTION_FRAME);

	++actionFrame;
	if (actionFrame == dodgeMax) {
		if (c[ControllerBits::LEFT] || c[ControllerBits::RIGHT]) {
			data->get<uint32_t>(STATE) = PlayerState::walking;
		}
		else {
			data->get<uint32_t>(STATE) = PlayerState::idle;
		}
		return;
	}

	auto vel = physics->getVel();
	vel.x = dodgeSpeed * data->get<int32_t>(DODGE_DIR);
	physics->setVel(vel);
}

void PlayerComponent::beginGroundedAttack() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	for (auto& attack : attacks) {
		attack.start();
		attack.setPos(physics->position(), data->get<int32_t>(DIR));
	}
	data->get<bool>(BUFFER_ATTACK) = false;
	data->get<uint32_t>(CURR_ATTACK) = PlayerAttack::slash1;
	data->get<uint32_t>(STATE) = PlayerState::grounded_attack;
}

void PlayerComponent::_groundedAttack(const Game& game) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	uint32_t& currAttack = data->get<uint32_t>(CURR_ATTACK);
	if (currAttack == PlayerAttack::none) {
		data->get<uint32_t>(STATE) = PlayerState::idle;
		return;
	}
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	Attack& attack = attacks[currAttack];
	attack.setPos(physics->position(), data->get<int32_t>(DIR));
	attack.update(game.PHYSICS_STEP * 1000);
	if (attack.finished()) {
		if (data->get<bool>(BUFFER_ATTACK)) {
			switch (currAttack) {
			case PlayerAttack::slash1: currAttack = PlayerAttack::slash2; break;
			case PlayerAttack::slash2: currAttack = PlayerAttack::slash3; break;
			case PlayerAttack::slash3: currAttack = PlayerAttack::none; break;
			}
			attack.setCurrHitboxIndex(0);
			data->get<bool>(BUFFER_ATTACK) = false;
		}
		else {
			attack.setCurrHitboxIndex(0);
			currAttack = PlayerAttack::none;
		}
	}
	else {
		ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
		auto& c = cont->getController();
		if (c.toggled(ControllerBits::BUTTON_11) && c[ControllerBits::BUTTON_11]) {
			data->get<bool>(BUFFER_ATTACK) = true;
		}
	}

	decelerate();
}

void PlayerComponent::beginRespawn() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	physics->setVel({ 0, 0 });
	data->get<uint32_t>(STATE) = PlayerState::respawning;
}

void PlayerComponent::_respawning(const Game& game)
{
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(STATE) = PlayerState::idle;
}

void PlayerComponent::beginHitstun() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(ACTION_FRAME) = 0;
	data->get<uint32_t>(STATE) = PlayerState::hitstun;

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	physics->setVel({ 0, 0 });
}

void PlayerComponent::_hitstun() {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	auto& actionFrame = data->get<uint32_t>(ACTION_FRAME);

	if (actionFrame >= 30) {
		data->get<uint32_t>(ACTION_FRAME) = 0;
		data->get<uint32_t>(STATE) = PlayerState::idle;
	}
	else {
		++actionFrame;
	}
}

void PlayerComponent::beginFreeze() {

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(FREEZE_STATE) = data->get<uint32_t>(STATE);
	data->get<uint32_t>(FREEZE_FRAME) = 0;
	data->get<uint32_t>(STATE) = freeze;

	auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	physics->freeze();
}

void PlayerComponent::_freeze() {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	auto& freezeFrame = data->get<uint32_t>(FREEZE_FRAME);

	if (freezeFrame >= 20) {
		data->get<uint32_t>(STATE) = data->get<uint32_t>(FREEZE_STATE);
		
		auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);
		physics->unfreeze();
	}
	else {
		++freezeFrame;
	}
}

void PlayerComponent::decelerate() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	auto vel = physics->getVel();

	if (vel.x > groundedDecel / 2) {
		physics->accelerate({ -groundedDecel, 0 });
	}
	else if (vel.x < -groundedDecel / 2) {
		physics->accelerate({ groundedDecel, 0 });
	}
	else {
		physics->setVel({ 0, vel.y });
	}
}
