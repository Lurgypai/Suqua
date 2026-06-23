#include "EntityAddDaemon.h"
#include "DaemonComponent.h"

void EntityAddDaemon(EntityId id, const EntityAddDaemonArgs& args) {
	EntitySystem::MakeComps<DaemonComponent>(1, &id, args.followRate, args.followOffset);
}