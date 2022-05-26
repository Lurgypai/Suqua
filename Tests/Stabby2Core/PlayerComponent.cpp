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
	id{id_},
	jumpSquatMax{ 8 },
	landingMax{ 8 },
	jumpVel{ 350 },
	shortHopVel{220},
	moveSpeed{ 80 },
	groundedDecel{ 10 },
	dodgeSpeed{ 100 },
	dodgeMax{56},
	airdodgeMax{ 30 },
	airdodgeSpeed{200}
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
		data->set<float>(AIRDODGE_X_DIR, 0);
		data->set<float>(AIRDODGE_Y_DIR, 0);
		data->set<bool>(FULLHOP, false);
		data->set<bool>(AIRDODGED, false);
		data->set<bool>(BUFFER_ATTACK, false);
		data->set<uint32_t>(STATE, PlayerState::idle);
		data->set<uint32_t>(CURR_ATTACK, PlayerAttack::none);
		data->set<uint32_t>(CURR_HITBOX, 0);
		data->set<uint32_t>(ATTACK_ELAPSED_TIME, 0);
	}

	attacks.resize(4);
	attacks[PlayerAttack::slash1].addHitbox({ AABB{ {0, -21}, {16, 13} } }, 300, 100, 45);
	attacks[PlayerAttack::slash2].addHitbox({ AABB{ {4, -23}, {16, 22} } }, 350, 150, 45);
	attacks[PlayerAttack::slash3].addHitbox({ AABB{ {8, -17, }, { 19, 17 } }, AABB{ {-3, -24}, {25, 11} } }, 400, 400, 45);
	attacks[PlayerAttack::dair].addHitbox({ AABB{ {0, -16}, {16, 16} } }, 400, 200, 45);
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
	}

	switch (data->get<uint32_t>(STATE)) {
	case PlayerState::idle: _idle(); break;
	case PlayerState::walking: _walking(); break;
	case PlayerState::jumpsquat: _jumpsquat(); break;
	case PlayerState::airborn: _airborn(); break;
	case PlayerState::landing: _landing(); break;
	case PlayerState::dodge: _dodge(); break;
	case PlayerState::airdodge: _airdodge(); break;
	case PlayerState::grounded_attack: _groundedAttack(game); break;
	}

	//set the synchronized currHitboxIndex
	if (currAttack != PlayerAttack::none) {
		data->get<uint32_t>(CURR_HITBOX) = attacks[currAttack].getCurrHitboxIndex();
		data->get<uint32_t>(ATTACK_ELAPSED_TIME) = attacks[currAttack].getElapsedTime();
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


	if (c[ControllerBits::LEFT] || c[ControllerBits::RIGHT]) {
		data->get<uint32_t>(STATE) = PlayerState::walking;
		return;
	}
	if (c[ControllerBits::BUTTON_5]) {
		beginJumpsquat();
		return;
	}
	if (c.toggled(ControllerBits::BUTTON_11) && c[ControllerBits::BUTTON_11]) {
		beginGroundedAttack();
		return;
	}
	if (!physics->isGrounded()) {
		data->get<uint32_t>(STATE) = PlayerState::airborn;
		return;
	}
	decelerate();
}

void PlayerComponent::_walking() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	if (c[ControllerBits::BUTTON_5]) {
		beginJumpsquat();
		return;
	}

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

inline void PlayerComponent::beginJumpsquat() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(ACTION_FRAME) = 0;
	data->get<bool>(FULLHOP) = true;
	data->get<uint32_t>(STATE) = PlayerState::jumpsquat;
}

void PlayerComponent::_jumpsquat() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	auto& c = cont->getController();
	if (!c[ControllerBits::BUTTON_5]) data->get<bool>(FULLHOP) = false;
	uint32_t& actionFrame = data->get<uint32_t>(ACTION_FRAME);
	bool& fullHop = data->get<bool>(FULLHOP);

	++actionFrame;
	if (actionFrame == jumpSquatMax) {
		physics->setVel({ physics->getVel().x, fullHop ? -jumpVel : -shortHopVel });
		data->get<uint32_t>(STATE) = PlayerState::airborn;
		return;
	}
	decelerate();
}

void PlayerComponent::_airborn() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (physics->isGrounded()) {
		data->get<uint32_t>(ACTION_FRAME) = 0;
		data->get<uint32_t>(STATE) = PlayerState::landing;
		return;
	}

	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	if (c[ControllerBits::BUTTON_6]) {
		if (!data->get<bool>(AIRDODGED)) {
			beginAirdodge();
		}
		return;
	}

	int dir = 0;
	if (c[ControllerBits::LEFT]) --dir;
	if (c[ControllerBits::RIGHT]) ++dir;

	auto vel = physics->getVel();
	vel.x = dir * moveSpeed;

	physics->setVel(vel);
}

void PlayerComponent::_landing() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	uint32_t& actionFrame = data->get<uint32_t>(ACTION_FRAME);

	++actionFrame;
	if (actionFrame == landingMax) {
		if (c[ControllerBits::LEFT] || c[ControllerBits::RIGHT]) {
			data->get<uint32_t>(STATE) = PlayerState::walking;
		}
		else if (c[ControllerBits::BUTTON_5]) {
			beginJumpsquat();
		}
		else if (c.toggled(ControllerBits::BUTTON_11) && c[ControllerBits::BUTTON_11]) {
			beginGroundedAttack();
		}
		else {
			data->get<uint32_t>(STATE) = PlayerState::idle;
		}
		return;
	}
	data->get<bool>(AIRDODGED) = false;
	decelerate();
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
		else if (c[ControllerBits::BUTTON_5]) {
			beginJumpsquat();
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

void PlayerComponent::beginAirdodge() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);

	auto& c = cont->getController();
	Vec2f dir{ 0, 0 };
	if (c[ControllerBits::LEFT]) --dir.x;
	if (c[ControllerBits::RIGHT]) ++dir.x;
	if (c[ControllerBits::UP]) --dir.y;
	if (c[ControllerBits::DOWN]) ++dir.y;

	Vec2f airdodgeDir = dir.norm();
	data->get<float>(AIRDODGE_X_DIR) = airdodgeDir.x;
	data->get<float>(AIRDODGE_Y_DIR) = airdodgeDir.y;


	physics->setWeigtless(true);

	data->get<uint32_t>(ACTION_FRAME) = 0;
	data->get<bool>(AIRDODGED) = true;
	data->get<uint32_t>(STATE) = PlayerState::airdodge;
}

void PlayerComponent::_airdodge() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	uint32_t& actionFrame = data->get<uint32_t>(ACTION_FRAME);

	++actionFrame;
	if (actionFrame == airdodgeMax) {
		physics->setWeigtless(false);
		data->get<uint32_t>(STATE) = PlayerState::airborn;
		return;
	}

	Vec2f airdodgeDir = {
		data->get<float>(AIRDODGE_X_DIR),
		data->get<float>(AIRDODGE_Y_DIR)
	};
	physics->setVel(airdodgeDir.scale(airdodgeSpeed));
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

void PlayerComponent::decelerate() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (physics->getVel().x > groundedDecel / 2) {
		physics->accelerate({ -groundedDecel, 0 });
	}
	else if (physics->getVel().x < -groundedDecel / 2) {
		physics->accelerate({ groundedDecel, 0 });
	}
	else {
		physics->setVel({ 0, 0 });
	}
}
