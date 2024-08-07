#include "AITopDownBasic.h"
#include "SDL.h"
#include "RandomUtil.h"
#include "PhysicsComponent.h"
#include "DebugIO.h"

#include <iostream>

AITopDownBasic::AITopDownBasic(InputDeviceId id_) :
	InputDevice{ id_ },
	controller{},
	stateTick{ 0 },
	idleTickMax{1000},
	walkTickMax{ 120 },
	followTickMax{ 480 },
	attackTickMax{ 480 },
	state{AIState::idle},
	followRadius{ 120 },
	targetId{ 0 }
{}

Controller AITopDownBasic::getControllerState() {
	return controller;
}

void AITopDownBasic::update() {
	if (stateTick % 20 == 0) {
		EntityId target = findTarget();
		switch (state) {
		case AIState::idle:
		case AIState::walking:
			if (target != 0) {
				state = AIState::following;
				stateTick = 0;
				targetId = target;
			} break;
		case AIState::following:
		case AIState::attacking:
			// std::cout << "attack/follow " << target << '\n';;
			if (target == 0) {
				state = AIState::idle;
				stateTick = 0;
				targetId = 0;
				// std::cout << "reset?\n";
			} break;
		}
	}

	switch (state) {
	case AIState::idle:
		controller.stick1 = { 0, 0 };
		if (stateTick == idleTickMax) {
			stateTick = 0;
			state = AIState::walking;
			controller.stick1 = Vec2f{ 0.3f, 0.0f };
			constexpr float PI = 3.1415926535898;
			float randDir = randFloat(0, PI * 2);
			controller.stick1.angle(randDir);
		}
		break;
	case AIState::walking:
		if (stateTick == walkTickMax) {
			stateTick = 0;
			state = AIState::idle;
			controller.stick1 = { 0, 0 };
		}
		break;
	case AIState::following: {
		auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(entityId);
		auto targetPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(targetId);
		auto dir = targetPhysicsComp->position() - physicsComp->position();
		controller.stick1 = dir.norm() * 0.7f;
		controller.stick2 = dir.norm();
		controller.set(ControllerBits::BUTTON_11, 0);

		DebugIO::setLine(3, "Follow Tick: " + std::to_string(stateTick) + " / " + std::to_string(followTickMax));
		DebugIO::setLine(4, "Stick2 Dir: " + std::to_string(controller.stick2.x) + ", " + std::to_string(controller.stick2.y));

		if (stateTick == followTickMax) {
			stateTick = 0;
			state = AIState::attacking;
			controller.stick1 = Vec2f{ 0, 0 };
		}

	} break;
	case AIState::attacking:
		auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(entityId);
		auto targetPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(targetId);
		auto dir = targetPhysicsComp->position() - physicsComp->position();
		controller.stick1 = Vec2f{ 0, 0 };
		controller.stick2 = dir.norm();
		controller.set(ControllerBits::BUTTON_11, 1);

		DebugIO::setLine(3, "AttackTick Tick: " + std::to_string(stateTick) + " / " + std::to_string(attackTickMax));

		if (stateTick == attackTickMax) {
			stateTick = 0;
			state = AIState::following;
		}
		break;
	}

	++stateTick;
}

void AITopDownBasic::setTargetTeams(std::vector<TeamComponent::TeamId> targetTeams_) {
	targetTeams = targetTeams_;
}

EntityId AITopDownBasic::findTarget() {
	for (auto teamComp : EntitySystem::GetPool<TeamComponent>()) {
		for (auto targetTeamId : targetTeams) {
			if (teamComp.teamId == targetTeamId) {
				auto targetPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(teamComp.getId());
				auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(entityId);

				float distance = targetPhysicsComp->position().distance(physicsComp->position());
				if (distance < followRadius) {
					return targetPhysicsComp->getId();
				}
			}
		}
	}
	return 0;
}

AITopDownBasic::AIState AITopDownBasic::getState() const {
	return state;
}