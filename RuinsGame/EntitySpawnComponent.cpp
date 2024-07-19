#include "EntitySpawnComponent.h"

EntitySpawnComponent::EntitySpawnComponent(EntityId id_) :
    id{id_},
    spawnTag{"none"},
    mode{SpawnMode::enter}
{}
