#include "EntitySpawnSystem.h"

using UUID = Suqua::UUID;

std::unique_ptr<EntitySpawnSystem::Generator> EntitySpawnSystem::generator{nullptr};

EntityId EntitySpawnSystem::SpawnEntity(
        const std::string& tag,
        Scene& scene,
        const Vec2f& pos,
        NetworkDataComponent::Owner owner,
        const UUID& uuid) {
    return generator->SpawnEntity(tag, scene, pos, owner, uuid);
}
