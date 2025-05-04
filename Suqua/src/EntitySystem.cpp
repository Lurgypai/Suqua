#include "EntitySystem.h"
#include "EntityBaseComponent.h"

EntityId EntitySystem::entityIncrem{ 0 };

void EntitySystem::GenEntities(unsigned int num, EntityId* idStore) {
	for (int i = 0; i != num; i++) {
		idStore[i] = ++entityIncrem;
		PoolMap::add<EntityBaseComponent>(idStore[i] - 1, idStore[i]);
	}
}

void EntitySystem::FreeEntities(unsigned int num, const EntityId* idStore) {
	for (int i = 0; i != num; i++) {
		for (auto& pool : PoolMap::getPools()) {
			pool->free(idStore[i] - 1);
		}
	}
}

void EntitySystem::FreeDeadEntities() {
    if(!EntitySystem::Contains<EntityBaseComponent>()) return;

	auto& pool = EntitySystem::GetPool<EntityBaseComponent>();
    for(auto& comp : pool) {
        if(!comp.isDead) continue;

        for(auto& pool : PoolMap::getPools()) pool->free(comp.getId() - 1);
    }
}
