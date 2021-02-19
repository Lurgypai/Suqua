#include "../player/AIPlayerComponent.h"
#include "../player/spawn/RespawnComponent.h"
#include "../combat/CombatComponent.h"
#include "../gamemode/CapturePointComponent.h"
#include "ControllerComponent.h"
#include "RandomUtil.h"


#include <iostream>

AIPlayerComponent::AIPlayerComponent(EntityId id_) :
	id{ id_ },
	currZone{},
	targetZone{},
	timer{0},
	state{AIPlayerComponent::State::capture},
	posTargeted{false}
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
		switch (state) {
		case State::defend:
		{
			if (!posTargeted) {
				std::vector<CapturePointComponent*> cappedPoints;
				for (auto& cp : EntitySystem::GetPool<CapturePointComponent>()) {
					if (cp.getState().currTeamId == ourCombat->teamId) {
						cappedPoints.push_back(&cp);
					}
				}
				if (!cappedPoints.empty()) {
					unsigned int spawn = randInt(0, cappedPoints.size() - 1);

					float xOffset = randFloat(0, cappedPoints[spawn]->getZone().res.x);
					targetPos = cappedPoints[spawn]->getZone().center() + Vec2f{ xOffset, 0 };
					targetZone = *navMesh.getZone(targetPos);
					targetId = cappedPoints[spawn]->getId();

					posTargeted = true;
				}
				else {
					chooseNewState();
				}
			}
			else {
				if (ourPos.distance(targetPos) < 10) {
					if (timer % (15 * 120) == 0) {
						chooseNewState();
					}
				}
			}
			break;
		}

		case State::capture:
		{
			if (!posTargeted) {
				std::vector<CapturePointComponent*> uncappedPoints;
				for (auto& cp : EntitySystem::GetPool<CapturePointComponent>()) {
					if (cp.getState().currTeamId != ourCombat->teamId) {
						uncappedPoints.push_back(&cp);
					}
				}
				if (!uncappedPoints.empty()) {
					unsigned int spawn = randInt(0, uncappedPoints.size() - 1);

					float xOffset = randFloat(0, uncappedPoints[spawn]->getZone().res.x);
					targetPos = uncappedPoints[spawn]->getZone().center() + Vec2f{xOffset, 0};
					targetZone = *navMesh.getZone(targetPos);
					targetId = uncappedPoints[spawn]->getId();

					posTargeted = true;
				}
				else {
					chooseNewState();
				}
			}
			else {
				if (ourPos.distance(targetPos) < 10) {
					CapturePointComponent* cp = EntitySystem::GetComp<CapturePointComponent>(targetId);
					if (cp->getState().currTeamId == ourCombat->teamId) {
						chooseNewState();
					}
				}
			}
			break;
		}
		case State::wander:
		{
			if (!posTargeted) {
				targetZone = std::next(std::begin(navMesh.getZones()), randInt(0, navMesh.getZones().size() - 1))->second;
				float xOffset = randFloat(0, targetZone.area.res.x);
				targetPos = targetZone.area.center() + Vec2f{xOffset, 0};
				targetId = 0;

				posTargeted = true;
			}
			else {
				if (ourPos.distance(targetPos) < 10) {
					if (timer % (15 * 120) == 0) {
						chooseNewState();
					}
				}
			}
			break;
		}
		}
		//add rule for entering combat from the other states here

		//calculate path to target position
		if (!targetZone.area.contains(targetPos)) {
			const NavZone* targetZonePtr = navMesh.getZone(targetPos);
			if (!targetZonePtr)
				return;
			targetZone = *targetZonePtr;
			if (currZone.id != 0 && targetZone.id != 0)
				currPath = navMesh.getPath(currZone.id, targetZone.id);
		}

		//update path based on our position
		if (!currZone.area.contains(ourPos.round())) {
			bool recalc = false;
			if (!currPath.empty()) {
				currPath.next();
				const NavZone& nextZone = navMesh.getZone(currPath.getCurrZoneId());
				if (nextZone.area.contains(ourPos)) {
					currZone = nextZone;
				}
				else {
					recalc = true;
				}
			}
			else {
				recalc = true;
			}

			if(recalc) {
				const NavZone* zonePtr = navMesh.getZone(ourPos);
				if (!zonePtr)
					return;
				currZone = *zonePtr;
				if (currZone.id != 0 && targetZone.id != 0)
					currPath = navMesh.getPath(currZone.id, targetZone.id);
			}
		}

		controller->getController().off(ControllerBits::ALL);

		//only travel if far enough away
		if (ourPos.distance(targetPos) > 10) {
			if (!currPath.finished()) {
				NavZone nextZone = navMesh.getZone(currPath.getNextZoneId());
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
							if (nextZone.typeTag == "walk") {
								controller->getController().set(ControllerBits::BUTTON_4, true);
							}
							else if (nextZone.typeTag == "climb") {
								controller->getController().set(ControllerBits::BUTTON_4, true);
								controller->getController().set(ControllerBits::UP, true);
							}
						}
					}
					//right
					else if (nextPoint.x > ourPos.x) {
						controller->getController().set(ControllerBits::RIGHT, true);
						if (nextLeft - ourPos.x < 4 && nextBottom < currZone.area.pos.y + currZone.area.res.y) {
							if (nextZone.typeTag == "walk") {
								controller->getController().set(ControllerBits::BUTTON_4, true);
							}
							else if (nextZone.typeTag == "climb") {
								controller->getController().set(ControllerBits::BUTTON_4, true);
								controller->getController().set(ControllerBits::UP, true);
							}
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

		EntityId spawnId{0};
		bool foundSpawn = false;

		const auto& spawns = respawner->getSpawnList();
		if (!respawner->getSpawnList().empty()) {
			int spawn = randInt(0, spawns.size() - 1);
			auto iter = spawns.begin();
			for (int i = 0; i != spawn; ++i) {
				++iter;
			}
			spawnId = *iter;
			foundSpawn = true;
		}

		if (foundSpawn) {
			SpawnComponent* spawnComp = EntitySystem::GetComp<SpawnComponent>(spawnId);
			playerComp->respawn(spawnComp->findSpawnPos());
		}
	}
}



const NavPath& AIPlayerComponent::getCurrentPath() const
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

void AIPlayerComponent::chooseNewState() {


}

ClimbableNavMesh AIPlayerComponent::navMesh{ 30, 4, 10 };
bool AIPlayerComponent::meshGenerated{ false };