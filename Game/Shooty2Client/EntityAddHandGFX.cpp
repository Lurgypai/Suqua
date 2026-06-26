#include "EntityAddHandGFX.h"
#include "HandItemGFXComponent.h"

void EntityAddHandGFX(EntityId id, const EntityAddHandGFXArgs& args) {
    EntitySystem::MakeComps<HandItemGFXComponent>(1, &id);
}
