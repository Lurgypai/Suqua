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
	auto& pool = EntitySystem::GetPool<EntityBaseComponent>();
	for (auto resIter = pool.beginResource(); resIter != pool.endResource(); ++resIter) {
		if (resIter->val.isDead) {
			for (auto& pool : ComponentMaps) {
				//add an erase function to do in place erasing
				pool->free(resIter->val.getId() - 1);
			}
		}
	}
}
