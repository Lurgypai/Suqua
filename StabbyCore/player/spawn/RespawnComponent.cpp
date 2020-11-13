#include "RespawnComponent.h"
#include "../../combat/CombatComponent.h"
#include "ControllerComponent.h"

RespawnComponent::RespawnComponent(EntityId id_) :
	id{id_},
	prevL{false},
	prevR{ false },
	prevSelect{false},
	state{unselected},
	selection{}
{}

EntityId RespawnComponent::getId() const {
	return id;
}

RespawnState RespawnComponent::getState() const {
	return state;
}

void RespawnComponent::loadSpawnList(SpawnSystem& spawns) {
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	spawnList.clear();
	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		if (spawn.getTeamId() == combat->teamId) {
			spawnList.insert(spawn.getId());
		}
	}

	if (!spawnList.empty()) {
		selection = spawnList.begin();
		currentSpawnZone = *selection;
		state = selecting;
	}
	else {
		state = none;
	}
}

void RespawnComponent::updateSpawnList(SpawnSystem& spawns) {
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	std::set<EntityId> currentSpawns{};
	bool foundCurrentSelection = false;

	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		if (spawn.getTeamId() == combat->teamId) {
			currentSpawns.insert(spawn.getId());
			if (spawn.getId() == currentSpawnZone)
				foundCurrentSelection = true;
		}
	}
	if (!foundCurrentSelection) {
		spawnList = std::move(currentSpawns);
		selection = spawnList.begin();
		currentSpawnZone = *selection;
	}
	else {
		spawnList = std::move(currentSpawns);
		selection = spawnList.find(currentSpawnZone);
	}
}

void RespawnComponent::searchForSpawn() {
	switch (state)
	{
	case unselected:
		break;
	case selecting: {
		ControllerComponent* contComp = EntitySystem::GetComp<ControllerComponent>(id);
		auto& controller = contComp->getController();

		bool currL = controller[ControllerBits::LEFT];
		bool currR = controller[ControllerBits::RIGHT];
		bool currSelect = controller[ControllerBits::BUTTON_2];

		int dir = 0;

		if (prevL != currL) {
			if (currL) {
				--dir;
			}
		}

		if (prevR != currR) {
			if (currR) {
				++dir;
			}
		}

		if (dir == 1) {
			++selection;

			if (selection == spawnList.end())
				selection = spawnList.begin();
		}
		if (dir == -1) {
			if (selection == spawnList.begin())
				selection = spawnList.end();

			--selection;
		}

		currentSpawnZone = *selection;

		if (prevSelect != currSelect) {
			if (currSelect) {
				state = selected;
			}
		}

		prevSelect = currSelect;
		prevL = currL;
		prevR = currR;
	}
		break;
	case selected:
		break;
	default:
		break;
	}
}

EntityId RespawnComponent::getCurrentSpawn() {
	return currentSpawnZone;
}

const std::set<EntityId>& RespawnComponent::getSpawnList() const {
	return spawnList;
}

void RespawnComponent::reset() {
	state = unselected;
}
