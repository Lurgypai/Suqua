#include "PlayerLC.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"
#include "../combat/CombatData.h"
#include "DebugIO.h"
#include "DirectionData.h"
#include "PlayerData.h"
#include "../stage/ClimbableComponent.h"
#include "../player/spawn/RespawnComponent.h"

#include <iostream>
#include <ControllerComponent.h>

using NDC = NetworkDataComponent;

PlayerLC::PlayerLC(EntityId id_) :
	id{id_},
	jumpSpeed{ 120 },
	isBeingHit{false},
	rollVel{150},
	storedVel{0},
	rollFrameMax{54},
	deathFrameMax{1200},
	healFrameMax{75},
	healDelayMax{120},
	horizontalAccel{10.0},
	stepDistance{70},
	climbDistance{35},
	rollCost{200},
	rollBuffered{false}
{
	//do not default construct
	if (id != 0) {
		if (!EntitySystem::Contains<NDC>() || EntitySystem::GetComp<NDC>(id) == nullptr) {
			EntitySystem::MakeComps<NDC>(1, &id);
			NDC* data = EntitySystem::GetComp<NDC>(id);

			data->set<float>(X, 0.0f);
			data->set<float>(Y, 0.0f);

			data->set<char>(STATE, static_cast<char>(State::free));
			data->set<uint32_t>(ROLL_FRAME, 0);
			data->set<uint32_t>(ACTIVE_ATTACK, 0);
			data->set<uint32_t>(ATTACK_FRAME, 0);
			data->set<uint32_t>(HEAL_FRAME, 0);
			data->set<uint32_t>(HEAL_DELAY, 0);
			data->set<uint32_t>(ATTACK_FREEZE_FRAME, 0);
			data->set<uint32_t>(DEATH_FRAME, 0);

			data->set<double>(ATTACK_SPEED, 1.0);
			data->set<double>(MOVE_SPEED, 1.0);
			data->set<std::string>(WEAPON_TAG, std::string{"sword"});
			data->set<std::string>(USER_TAG, std::string{"grqgrcqy"});

			data->set<int32_t>(HEALTH, 100);
			data->set<int32_t>(DIR, -1);
		}
		if (!EntitySystem::Contains<PhysicsComponent>() || EntitySystem::GetComp<PhysicsComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
			NDC* data = EntitySystem::GetComp<NDC>(id);
			physics->setRes(Vec2f{ static_cast<float>(PlayerLC::PLAYER_WIDTH), static_cast<float>(PlayerLC::PLAYER_HEIGHT) });

			data->get<float>(WEIGHT) = 3;
			data->get<float>(X) = -2;
			data->get<float>(Y) = -20;
			data->get<int32_t>(DIR) = -1;


		}
		if (!EntitySystem::Contains<CombatComponent>() || EntitySystem::GetComp<CombatComponent>(id) == nullptr) {
			EntitySystem::MakeComps<CombatComponent>(1, &id);
		}
		if (!EntitySystem::Contains<ControllerComponent>() || EntitySystem::GetComp<ControllerComponent>(id) == nullptr) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}
		if (!EntitySystem::Contains<RespawnComponent>() || EntitySystem::GetComp<RespawnComponent>(id) == nullptr) {
			EntitySystem::MakeComps<RespawnComponent>(1, &id);
		}
	}
}

void PlayerLC::update(double timeDelta) {

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);
	ControllerComponent* contPtr = EntitySystem::GetComp<ControllerComponent>(id);
	NDC* data = EntitySystem::GetComp<NDC>(id);
	const State state = static_cast<PlayerLC::State>(data->get<char>(STATE));

	auto& controller = contPtr->getController();
	const Attack & attack = combat->getAttack();
	


	bool attackToggledDown{ false };
	bool currButton2 = controller[ControllerBits::BUTTON_2];
	if (controller.toggled(ControllerBits::BUTTON_2)) {
		if (currButton2) {
			attackToggledDown = true;
			if (state == State::attacking) {
				combat->bufferNextAttack();
			}
		}
	}

	const Vec2f vel{data->get<float>(XVEL), data->get<float>(YVEL)};
	data->get<bool>(WEIGHTLESS) = false;

	if (!combat->isFrozen()) {
		switch (state) {
		case State::stunned:
			if (combat->isStunned()) {
				data->get<float>(XVEL) = 0;

				bool currButton3 = controller[ControllerBits::BUTTON_3];
				if (controller.toggled(ControllerBits::BUTTON_3)) {
					if (currButton3) {
						rollBuffered = true;
					}
				}
			}
			else {
				data->get<char>(STATE) = static_cast<char>(State::free);
			}
			break;
		case State::attacking:
			data->get<float>(XVEL) = 0;
			if (attack.getActiveId() == 0)
				data->get<char>(STATE) = static_cast<char>(State::free);
			break;
		case State::rolling:
			if (data->get<uint32_t>(ROLL_FRAME) != rollFrameMax) {
				data->get<uint32_t>(ROLL_FRAME)++;

				int dir{ 0 };
				if (controller[ControllerBits::LEFT]) {
					--dir;
				}
				if (controller[ControllerBits::RIGHT]) {
					++dir;
				}

				data->get<float>(XVEL) = (rollVel * data->get<double>(MOVE_SPEED) * data->get<int32_t>(DIR)) + (dir * 40 * data->get<double>(MOVE_SPEED));
			}
			else {
				data->get<bool>(INVULNERABLE) = false;
				data->get<uint32_t>(ROLL_FRAME) = 0;
				data->get<float>(XVEL) = storedVel;
				data->get<char>(STATE) = static_cast<char>(State::free);
			}
			break;
		case State::free:
			free(controller, attackToggledDown);
			break;
		case State::dead:
			data->get<float>(XVEL) = 0;
			++data->get<uint32_t>(DEATH_FRAME);
			break;
		case State::crouching:
		{
			data->get<float>(XVEL) = 0;
			auto& healDelay = data->get<uint32_t>(HEAL_DELAY);
			auto& healFrame = data->get<uint32_t>(HEAL_FRAME);
			if (healDelay == healDelayMax) {
				healFrame++;
				if (healFrame == healFrameMax) {
					healFrame = 0;
					combat->heal(combat->stats.regeneration);
				}
			}
			else {
				++healDelay;
			}
			if (!controller[ControllerBits::DOWN])
				data->get<char>(STATE) = static_cast<char>(State::free);

			bool currButton3 = controller[ControllerBits::BUTTON_3];
			if (controller.toggled(ControllerBits::BUTTON_3)) {
				if (currButton3) {
					if (data->get<uint32_t>(STAMINA) >= rollCost) {
						data->get<char>(STATE) = static_cast<char>(State::rolling);
						data->get<bool>(INVULNERABLE) = true;
						storedVel = vel.x;
						data->get<float>(XVEL) = data->get<int32_t>(DIR) * rollVel;

						//remove stamina and restart timer to refill stamina
						combat->useStamina(rollCost);
						data->get<uint32_t>(STAMINA_RECHARGE_FRAME) = 0;
						break;
					}
				}
			}
			break;
		}
		case State::climbing:

			bool overlaps{false};
			for (auto& climbable : EntitySystem::GetPool<ClimbableComponent>()) {
				if (comp->intersects(climbable.collider)) {
					overlaps = true;
					break;
				}
			}

			data->get<bool>(WEIGHTLESS) = true;

			if (!overlaps) {
				data->get<char>(STATE) = static_cast<char>(State::free);
				break;
			}
			else if (controller[ControllerBits::BUTTON_4]) {
				data->get<char>(STATE) = static_cast<char>(State::free);
				break;
			}

			Vec2f dir{ 0, 0 };
			if (controller[ControllerBits::LEFT]) {
				--dir.x;
			}
			if (controller[ControllerBits::RIGHT]) {
				++dir.x;
			}
			if (controller[ControllerBits::UP]) {
				--dir.y;
			}
			if (controller[ControllerBits::DOWN]) {
				++dir.y;
			}

			auto vel = dir * static_cast<float>(climbDistance * data->get<double>(MOVE_SPEED));
			data->get<float>(XVEL) = vel.x;
			data->get<float>(YVEL) = vel.y;
			break;
		}

		if (data->get<float>(Y) > 1000) {
			kill();
		}
	}

	if (data->get<int32_t>(HEALTH) <= 0) {
		data->get<char>(STATE) = static_cast<char>(State::dead);
		rollBuffered = false;
	}
	else if (combat->isStunned())
		data->get<char>(STATE) = static_cast<char>(State::stunned);

	data->get<uint32_t>(ACTIVE_ATTACK) = attack.getActiveId();
	data->get<uint32_t>(ATTACK_FRAME) = attack.getCurrFrame();
}

PhysicsComponent * PlayerLC::getPhysics() {
	return EntitySystem::GetComp<PhysicsComponent>(id);
}

Vec2f PlayerLC::getVel() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return {data->get<float>(XVEL), data->get<float>(YVEL)};
}

Vec2f PlayerLC::getRes() const {
	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	return comp->getRes();
}

void PlayerLC::setWeapon(const std::string& weaponTag) {
	std::cout << "weapon set too: " << weaponTag << '\n';
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	combat->setAttack(weaponTag);

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<std::string>(WEAPON_TAG) = weaponTag;
}

void PlayerLC::respawn(const Vec2f & spawnPos) {
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	NDC* data = EntitySystem::GetComp<NDC>(id);

	data->get<int32_t>(HEALTH) = 100;
	data->get<uint32_t>(STUN_FRAME) = 0;
	data->get<uint32_t>(STAMINA) = combat->staminaMax;

	data->get<char>(STATE) = static_cast<char>(State::free);
	data->get<uint32_t>(ACTIVE_ATTACK) = 0;
	data->get<uint32_t>(ATTACK_FRAME) = 0;

	PhysicsComponent* comp = EntitySystem::GetComp<PhysicsComponent>(id);
	data->get<uint32_t>(DEATH_FRAME) = 0;

	comp->teleport(spawnPos);
	data->get<float>(XVEL) = 0;
	data->get<float>(YVEL) = 0;
	combat->setActiveHitbox(0);
	combat->setFrame(0);
}

bool PlayerLC::shouldRespawn() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<uint32_t>(DEATH_FRAME) >= deathFrameMax;
}

void PlayerLC::kill() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	if (static_cast<State>(data->get<char>(STATE)) != State::dead) {

		CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);

		data->get<char>(STATE) = static_cast<char>(State::dead);
		data->get<int32_t>(HEALTH) = 0;
		data->get<uint32_t>(DEATH_FRAME) = 0;
	}
}

void PlayerLC::chooseSpawn() {

	NDC* data = EntitySystem::GetComp<NDC>(id);
	if (static_cast<State>(data->get<char>(STATE)) != State::dead) {

		CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);

		data->get<char>(STATE) = static_cast<char>(State::dead);
		data->get<int32_t>(HEALTH) = 0;
		data->get<uint32_t>(DEATH_FRAME) = deathFrameMax;
	}
}

float PlayerLC::getRespawnProgress() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	float deathFrame = static_cast<float>(data->get<uint32_t>(DEATH_FRAME));
	if (deathFrame >= 0)
		return deathFrame / deathFrameMax;
	else
		return 0;
}

EntityId PlayerLC::getId() const {
	return id;
}

void PlayerLC::free(const Controller & controller, bool attackToggledDown_) {

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	NDC* data = EntitySystem::GetComp<NDC>(id);
	Vec2f vel{data->get<float>(XVEL), data->get<float>(YVEL)};

	CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);

	//always return after changing to a new state

	//start attacking
	if (attackToggledDown_) {
		if (combat->startAttacking()) {
			data->get<char>(STATE) = static_cast<char>(State::attacking);
			return;
		}
	}

	//try to start rolling
	if (static_cast<State>(data->get<char>(STATE)) != State::attacking) {
		bool currButton3 = controller[ControllerBits::BUTTON_3];
		if ((controller.toggled(ControllerBits::BUTTON_3) && currButton3) || rollBuffered) {
			rollBuffered = false;
			if (data->get<uint32_t>(STAMINA) >= rollCost) {
				data->get<char>(STATE) = static_cast<char>(State::rolling);
				data->get<bool>(INVULNERABLE) = true;
				storedVel = vel.x;
				vel.x = data->get<int32_t>(DIR) * rollVel;

				//remove stamina and restart timer to refill stamina
				combat->useStamina(rollCost);
				data->get<uint32_t>(STAMINA_RECHARGE_FRAME) = 0;
				return;
			}
		}
	}

	//start climbing
	if (EntitySystem::Contains<ClimbableComponent>()) {
		for (auto& climable : EntitySystem::GetPool<ClimbableComponent>()) {
			if (comp->intersects(climable.collider) && controller[ControllerBits::UP]) {
				data->get<char>(STATE) = static_cast<char>(State::climbing);
				data->get<bool>(WEIGHTLESS) = true;
				return;
			}
		}
	}

	if (data->get<bool>(GROUNDED) && controller[ControllerBits::DOWN]) {
		data->get<char>(STATE) = static_cast<char>(State::crouching);
		data->get<uint32_t>(HEAL_DELAY) = 0;
		data->get<uint32_t>(HEAL_FRAME) = 0;
		return;
	}

	if (data->get<char>(STATE) == static_cast<char>(State::free)) {
		int dir{ 0 };
		if (controller[ControllerBits::LEFT]) {
			--dir;
		}
		if (controller[ControllerBits::RIGHT]) {
			++dir;
		}
		if (controller[ControllerBits::BUTTON_4]) {
			if (data->get<bool>(GROUNDED)) {
				vel.y = -jumpSpeed* data->get<double>(MOVE_SPEED);
			}
		}
		//otherwise do
		if (dir != 0)
			data->get<int32_t>(DIR) = dir;

		float targetVel = dir * data->get<double>(MOVE_SPEED) * stepDistance;
		float accel = data->get<double>(MOVE_SPEED) * horizontalAccel;
		if (vel.x < targetVel - accel) {
			vel.x += accel;
		}
		else if (vel.x > targetVel + accel) {
			vel.x -= accel;
		}
		else {
			vel.x = targetVel;
		}
	}


	data->get<float>(XVEL) = vel.x;
	data->get<float>(YVEL) = vel.y;
}
