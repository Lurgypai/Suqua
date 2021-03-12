#include "RespawnComponent.h"
#include "../../combat/CombatComponent.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"
#include "../../combat/CombatData.h"

using NDC = NetworkDataComponent;

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

void RespawnComponent::beginSelecting() {
	loadSpawnList();
	if (!spawnList.empty()) {
		selection = spawnList.begin();
		currentSpawnZone = *selection;
		state = selecting;
	}
	else {
		state = none;
	}
}

void RespawnComponent::loadSpawnList() {
	if (state != selecting) {
		NDC* data = EntitySystem::GetComp<NDC>(id);
		spawnList.clear();
		for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
			if (spawn.getTeamId() == data->get<uint32_t>(TEAM_ID)) {
				spawnList.insert(spawn.getId());
			}
		}
	}
}

void RespawnComponent::updateSpawnList(SpawnSystem& spawns) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	std::set<EntityId> currentSpawns{};
	bool foundCurrentSelection = false;

	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		if (spawn.getTeamId() == data->get<uint32_t>(TEAM_ID)) {
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
