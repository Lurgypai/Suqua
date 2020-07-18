#include "../player/AIPlayerComponent.h"
#include "../combat/CombatComponent.h"
#include "ControllerComponent.h"

#include <iostream>

AIPlayerComponent::AIPlayerComponent(EntityId id_) :
	id{ id_ },
	currZone{}
{
	if (id != 0) {
		if (!meshGenerated) {
			navMesh.addPhysicsMesh();
			navMesh.addClimbableMesh();
			meshGenerated = true;
		}

		PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
		const NavZone* zonePtr = navMesh.getZone(physics->getPos() + Vec2f{ 0, -1 });
		if (zonePtr)
			currZone = *zonePtr;
	}
}

EntityId AIPlayerComponent::getId() const {
	return id;
}

void AIPlayerComponent::update() {
	ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
	
	CombatComponent* ourCombat = EntitySystem::GetComp<CombatComponent>(id);
	PhysicsComponent* ourPhysics = EntitySystem::GetComp<PhysicsComponent>(id);
	Vec2f ourPos = ourPhysics->getPos() + Vec2f{0, -1};
	Vec2f targetPos;

	for (auto& otherCombat : EntitySystem::GetPool<CombatComponent>()) {
		if (otherCombat.getId() != id && otherCombat.teamId != ourCombat->teamId) {
			PhysicsComponent* otherPhysics = EntitySystem::GetComp<PhysicsComponent>(otherCombat.getId());
			targetPos = otherPhysics->getPos() + Vec2f{0, -1};
		}
	}
	
	const NavZone* targetZonePtr = navMesh.getZone(targetPos);
	if (!targetZonePtr)
		return;
	targetZone = *targetZonePtr;

	if (!currZone.area.contains(ourPos)) {
		const NavZone* zonePtr = navMesh.getZone(ourPos);
		if (!zonePtr)
			return;
		currZone = *zonePtr;
	}

	currPath = navMesh.getPath(currZone.id, targetZone.id);

	controller->getController().off(ControllerBits::ALL);

	if (currPath.size() > 1) {
		auto iter = currPath.begin();
		NavZone nextZone = navMesh.getZone(*(++iter));
		Vec2f nextPoint = nextZone.area.center();


		float nextLeft = nextZone.area.pos.x;
		float nextRight = nextLeft + nextZone.area.res.x;
		float nextTop = nextZone.area.pos.y;
		float nextBottom = nextTop + nextZone.area.res.y;

		if (currZone.typeTag == "walk") {
			//left
			if (nextPoint.x < ourPos.x) {
				controller->getController().set(ControllerBits::LEFT, true);
				if (ourPos.x - nextRight < 4 && nextBottom < currZone.area.pos.y + currZone.area.res.y) {
					controller->getController().set(ControllerBits::UP, true);
				}
			}
			//right
			else if (nextPoint.x > ourPos.x) {
				controller->getController().set(ControllerBits::RIGHT, true);
				if (nextLeft - ourPos.x < 4 && nextBottom < currZone.area.pos.y + currZone.area.res.y) {
					controller->getController().set(ControllerBits::UP, true);
				}
			}
		}
		else if (currZone.typeTag == "climb") {
			if (ourPos.y > nextBottom - 3) {
				controller->getController().set(ControllerBits::UP, true);
			}
			else if (ourPos.y < nextTop) {
				controller->getController().set(ControllerBits::DOWN, true);
			}
			else {
				if (nextPoint.x < ourPos.x) {
					controller->getController().set(ControllerBits::LEFT, true);
				}
				else if (nextPoint.x > ourPos.x) {
					controller->getController().set(ControllerBits::RIGHT, true);
				}
			}
		}
	}
	else {
		if (targetPos.x < ourPos.x) {
			controller->getController().set(ControllerBits::LEFT, true);
		}
		else if (targetPos.x > ourPos.x) {
			controller->getController().set(ControllerBits::RIGHT, true);
		}
	}
}



const std::list<unsigned int>& AIPlayerComponent::getCurrentPath() const
{
	return currPath;
}

const NavZone& AIPlayerComponent::getCurrZone() const {
	return currZone;
}

const NavZone& AIPlayerComponent::getTargetZone() const {
	return targetZone;
}

const ClimbableNavMesh& AIPlayerComponent::getNavMesh() {
	return navMesh;
}

ClimbableNavMesh AIPlayerComponent::navMesh{ 30, 4, 10 };
bool AIPlayerComponent::meshGenerated{ false };