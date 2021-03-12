#include "EntitySystem.h"
#include "EntityBaseComponent.h"

EntityId EntitySystem::entityIncrem{0};
PoolMap EntitySystem::ComponentMaps{};

void EntitySystem::GenEntities(unsigned int num, EntityId * idStore) {
	for (int i = 0; i != num; i++) {
		idStore[i] = ++entityIncrem;
		ComponentMaps.add<EntityBaseComponent>(idStore[i] - 1, idStore[i]);
	}
}

void EntitySystem::FreeEntities(unsigned int num, const EntityId * idStore) {
	for (int i = 0; i != num; i++) {
		for (auto & pool : ComponentMaps) {
			pool->free(idStore[i] - 1);
		}
	}
}

void EntitySystem::FreeDeadEntities() {
	std::vector<EntityId> deadComps{};
	Pool<EntityBaseComponent> & entities = ComponentMaps.get<EntityBaseComponent>();
	deadComps.reserve(entities.size());
	for (auto& comp : entities) {
		if (comp.isDead) {
			deadComps.push_back(comp.getId());
		}
	}

	EntitySystem::FreeEntities(deadComps.size(), deadComps.data());
}
