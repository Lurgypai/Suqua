#include "EntitySpawnSystem.h"
#include <vector>

std::unique_ptr<EntitySpawnSystem::Generator> EntitySpawnSystem::generator{nullptr};

std::vector<EntityId> EntitySpawnSystem::SpawnEntity(
        const std::string& tag,
        Scene& scene,
        const Vec2f& pos,
        NetworkDataComponent::Owner owner,
        const std::vector<UUID>& uuids) {
    return generator->SpawnEntity(tag, scene, pos, owner, uuids);
}
