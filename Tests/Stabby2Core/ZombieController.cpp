#include "ZombieController.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "PlayerComponent.h"
#include "SDL.h"

ZombieController::ZombieController(InputDeviceId id_, EntityId entity_) :
	InputDevice{ id_ },
	entity{entity_}
{}

Controller ZombieController::getControllerState() {
	Controller controller;

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(entity);
	
	//find nearest player
	float minDist = followDist;
	PhysicsComponent* otherPhysics = nullptr;
	if (EntitySystem::Contains<PlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<PlayerComponent>()) {
			auto* testPhysics = EntitySystem::GetComp<PhysicsComponent>(player.getId());
			float dist = testPhysics->getPos().distance(physics->getPos());

			if (dist < minDist) {
				otherPhysics = testPhysics;
				minDist = dist;
			}
		}
	}

	//steer toward them
	if (otherPhysics) {
		if (otherPhysics->getPos().x < physics->getPos().x) {
			controller.set(ControllerBits::LEFT, true);
			controller.set(ControllerBits::RIGHT, false);
		}
		else if (otherPhysics->getPos().x > physics->getPos().x) {
			controller.set(ControllerBits::RIGHT, true);
			controller.set(ControllerBits::LEFT, false);
		}

		controller.set(ControllerBits::BUTTON_11, minDist < lungeDist);
	}
	else {
		controller.set(ControllerBits::RIGHT, false);
		controller.set(ControllerBits::LEFT, false);
		controller.set(ControllerBits::BUTTON_11, false);
	}

	return controller;
}