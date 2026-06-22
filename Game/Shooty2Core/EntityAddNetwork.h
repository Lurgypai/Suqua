#pragma once

#include "EntitySystem.h"
#include "UUID.h"
#include "NetworkDataComponent.h"

struct EntityAddNetworkArgs {
    const Suqua::UUID& uuid;
    NetworkDataComponent::Owner owner;
};

extern void EntityAddNetwork(EntityId id, const EntityAddNetworkArgs& args);
