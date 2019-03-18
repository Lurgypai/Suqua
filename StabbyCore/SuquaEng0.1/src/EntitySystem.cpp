#include "stdafx.h"
#include "EntitySystem.h"

EntityId EntitySystem::entityIncrem{0};
PoolMap EntitySystem::ComponentMaps{};

void EntitySystem::GenEntities(unsigned int num, unsigned int * idStore) {
	for (int i = 0; i != num; i++) {
		*(idStore + i) = ++entityIncrem;
	}
}
