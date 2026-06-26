#include "EntityAddCharacterGFX.h"
#include "CharacterGFXComponent.h"

void EntityAddCharacterGFX(EntityId id, const EntityAddCharacterGFXArgs& args) {
    EntitySystem::MakeComps<CharacterGFXComponent>(1, &id,
        args.texPath,
        args.animJsonPath,
        args.offset,
        args.shadowOffset,
        args.hasUpDown
        );
}
