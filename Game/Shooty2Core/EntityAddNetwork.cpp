#include "EntityAddNetwork.h"

void EntityAddNetwork(EntityId id, const EntityAddNetworkArgs& args) {
    EntitySystem::MakeComps<NetworkDataComponent>(1, &id, args.uuid, args.owner);
}
