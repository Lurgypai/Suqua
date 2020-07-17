#include "../player/AIPlayerComponent.h"
#include "../combat/CombatComponent.h"
#include "ControllerComponent.h"

AIPlayerComponent::AIPlayerComponent(EntityId id_) : id{ id_ }
{}

EntityId AIPlayerComponent::getId() const {
	return id;
}

void AIPlayerComponent::update() {
	ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
	
	CombatComponent* ourCombat = EntitySystem::GetComp<CombatComponent>(id);
	PhysicsComponent* ourPhysics = EntitySystem::GetComp<PhysicsComponent>(id);
	Vec2f ourPos = ourPhysics->getPos();
	Vec2f targetPos;

	for (auto& otherCombat : EntitySystem::GetPool<CombatComponent>()) {
		if (otherCombat.getId() != id && otherCombat.teamId != ourCombat->teamId) {
			PhysicsComponent* otherPhysics = EntitySystem::GetComp<PhysicsComponent>(otherCombat.getId());
			targetPos = otherPhysics->getPos();
		}
	}
	PhysicsComponent* rightBlock = nullptr;
	PhysicsComponent* leftBlock = nullptr;
	float viewDistance = 5;
	float minRightDistance = viewDistance;
	float minLeftDistance = viewDistance;
	for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
		if (physics.collideable) {
			const AABB& collider = physics.getCollider();
			//if between the top and bottom
			if (collider.pos.y < ourPos.y && collider.pos.y > ourPos.y - ourPhysics->getCollider().res.y) {
				if (collider.pos.x > ourPos.x) {
					float distance = collider.pos.x - ourPos.x;
					if (distance < minRightDistance) {
						minRightDistance = distance;
						rightBlock = &physics;
					}
				}
				else if (collider.pos.x + collider.res.x < ourPos.x) {
					float distance = ourPos.x - (collider.pos.x + collider.res.x);
					if (distance < minLeftDistance) {
						minLeftDistance = distance;
						leftBlock = &physics;
					}
				}
			}
		}
	}

	controller->getController().off(ControllerBits::ALL);

	if (targetPos.x < ourPos.x) {
		controller->getController().set(ControllerBits::LEFT, true);
		if (leftBlock != nullptr) {
			controller->getController().set(ControllerBits::UP, true);
		}
	}
	else if (targetPos.x > ourPos.x) {
		controller->getController().set(ControllerBits::RIGHT, true);
		if (rightBlock != nullptr) {
			controller->getController().set(ControllerBits::UP, true);
		}
	}
}
