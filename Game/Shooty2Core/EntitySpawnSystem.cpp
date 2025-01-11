#include "EntitySpawnSystem.h"
#include <vector>

std::unique_ptr<EntitySpawnSystem::Generator> EntitySpawnSystem::generator{nullptr};

std::vector<EntityId> EntitySpawnSystem::SpawnEntity(const std::string& tag, Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner, bool shared) {
    return generator->SpawnEntity(tag, scene, pos, owner, shared);
}
