#include "EntitySystem.h"
#include "EntityBaseComponent.h"

EntityId EntitySystem::entityIncrem{0};

void EntitySystem::GenEntities(unsigned int num, EntityId * idStore) {
	for (int i = 0; i != num; i++) {
		idStore[i] = ++entityIncrem;
		PoolMap::add<EntityBaseComponent>(idStore[i] - 1, idStore[i]);
	}
}

void EntitySystem::FreeEntities(unsigned int num, const EntityId * idStore) {
	for (int i = 0; i != num; i++) {
		for (auto& pool : PoolMap::getPools()) {
			pool->free(idStore[i] - 1);
		}
	}
}

void EntitySystem::FreeDeadEntities() {
	auto& pool = EntitySystem::GetPool<EntityBaseComponent>();
	if (EntitySystem::Contains<EntityBaseComponent>()) {
		for (auto resIter = pool.beginResource(); resIter != pool.endResource(); ++resIter) {
			if (resIter->val.isDead) {
				for (auto& pool : PoolMap::getPools()) {
					//add an erase function to do in place erasing
					pool->free(resIter->val.getId() - 1);
				}
			}
		}
	}
}
