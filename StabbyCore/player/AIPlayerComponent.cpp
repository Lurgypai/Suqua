#include "../player/AIPlayerComponent.h"
#include "../combat/CombatComponent.h"
#include "../gamemode/CapturePointComponent.h"
#include "../player/spawn/RespawnComponent.h"
#include "ControllerComponent.h"
#include "RandomUtil.h"


#include <iostream>

AIPlayerComponent::AIPlayerComponent(EntityId id_) :
	id{ id_ },
	currZone{},
	targetZone{},
	timer{0},
	findSpawn{true},
	state{AIPlayerComponent::State::defend}
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
	++timer;
	ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
	
	CombatComponent* ourCombat = EntitySystem::GetComp<CombatComponent>(id);
	PhysicsComponent* ourPhysics = EntitySystem::GetComp<PhysicsComponent>(id);
	PlayerLC* playerComp = EntitySystem::GetComp<PlayerLC>(id);

	Vec2f ourPos = ourPhysics->getPos() + Vec2f{0, -1};
	Vec2f targetPos{};
	if (!playerComp->shouldRespawn()) {
		findSpawn = true;

		switch (state) {
		case State::defend:
		{
			targetId = 0;
			bool attackingPlayer = false;
			for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
				CombatComponent* otherCombat = EntitySystem::GetComp<CombatComponent>(player.getId());
				if (otherCombat->isAlive()) {
					PhysicsComponent* otherPhysics = EntitySystem::GetComp<PhysicsComponent>(player.getId());
					if (otherCombat->teamId != ourCombat->teamId) {
						if (otherPhysics->getPos().distance(ourPos) < 100) {
							targetPos = otherPhysics->getPos();
							targetId = player.getId();
							attackingPlayer = true;
						}
					}
				}
			}

			if (!attackingPlayer) {
				//find the closest capture point
				float minDistance = -1;
				CapturePointComponent* targetPoint = nullptr;
				for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
					SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(capturePoint.getId());
					if (!spawn->isDefault()) {
						if (capturePoint.getState().currTeamId != ourCombat->teamId) {
							float distance = ourPos.distance(capturePoint.getZone().center());
							if (minDistance == -1 || distance < minDistance) {
								minDistance = distance;
								targetPoint = &capturePoint;
							}
						}
					}
				}
				//we have them all already
				if (!targetPoint) {
					minDistance = -1;
					for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
						SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(capturePoint.getId());
						if (!spawn->isDefault()) {
							if (capturePoint.getState().currTeamId == ourCombat->teamId) {
								float distance = ourPos.distance(capturePoint.getZone().center());
								if (minDistance == -1 || distance < minDistance) {
									minDistance = distance;
									targetPoint = &capturePoint;
								}
							}
						}
					}
				}
				float xOffset = randFloat(0, targetPoint->getZone().res.x);
				targetPos = { targetPoint->getZone().pos.x + xOffset, targetPoint->getZone().center().y };
			}
			break;
		}
		}

		if (!targetZone.area.contains(targetPos)) {
			const NavZone* targetZonePtr = navMesh.getZone(targetPos);
			if (!targetZonePtr)
				return;
			targetZone = *targetZonePtr;
			if (currZone.id != 0 && targetZone.id != 0)
				currPath = navMesh.getPath(currZone.id, targetZone.id);
		}

		if (!currZone.area.contains(ourPos.round())) {
			const NavZone* zonePtr = navMesh.getZone(ourPos);
			if (!zonePtr)
				return;
			currZone = *zonePtr;
			if (currZone.id != 0 && targetZone.id != 0)
				currPath = navMesh.getPath(currZone.id, targetZone.id);

		}

		controller->getController().off(ControllerBits::ALL);

		if (targetId) {
			if (ourPos.distance(targetPos) < 10) {
				if (timer % 2)
					controller->getController().set(ControllerBits::BUTTON_2, true);
			}
		}

		//only travel if far enough away
		if (std::abs(targetPos.x - ourPos.x) > 10) {
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
						if (std::abs(ourPos.x - currZone.area.center().x) < 1) {
							controller->getController().set(ControllerBits::UP, true);
						}
						else {
							if (currZone.area.center().x < ourPos.x) {
								controller->getController().set(ControllerBits::LEFT, true);
							}
							else if (currZone.area.center().x > ourPos.x) {
								controller->getController().set(ControllerBits::RIGHT, true);
							}
						}
					}
					else if (ourPos.y < nextTop) {
						if (std::abs(ourPos.x - currZone.area.center().x) < 1) {
							controller->getController().set(ControllerBits::DOWN, true);
						}
						else {
							if (currZone.area.center().x < ourPos.x) {
								controller->getController().set(ControllerBits::LEFT, true);
							}
							else if (currZone.area.center().x > ourPos.x) {
								controller->getController().set(ControllerBits::RIGHT, true);
							}
						}
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
	}
	else {
		controller->getController().set(ControllerBits::ALL, false);
		RespawnComponent* respawner = EntitySystem::GetComp<RespawnComponent>(id);
		if (findSpawn) {
			const auto& spawns = respawner->getSpawnList();
			if (!respawner->getSpawnList().empty()) {
				int spawn = randInt(0, spawns.size() - 1);
				auto iter = spawns.begin();
				for (int i = 0; i != spawn; ++i) {
					++iter;
				}
				targetId = *iter;
				findSpawn = false;
			}
		}
		else {
			if (respawner->getCurrentSpawn() != targetId) {
				controller->getController().set(ControllerBits::LEFT, timer % 4);
			}
			else {
				controller->getController().set(ControllerBits::BUTTON_2, true);
			}
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