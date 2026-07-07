#include "EntityAddTeleportGFX.h"
#include "TeleportZoneGFXComponent.h"

void EntityAddTeleportGFX(EntityId id, const EntityAddTeleportGFXArgs& args) {
    EntitySystem::MakeComps<TeleportZoneGFXComponent>(1, &id);
}
