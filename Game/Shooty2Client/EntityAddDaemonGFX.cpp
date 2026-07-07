#include "EntityAddDaemonGFX.h"
#include "DaemonGFXComponent.h"

void EntityAddDaemonGFX(EntityId id, const EntityAddDaemonGFXArgs& args) {
    EntitySystem::MakeComps<DaemonGFXComponent>(1, &id);
}
