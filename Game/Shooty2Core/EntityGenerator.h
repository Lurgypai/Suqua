#pragma once
#include <unordered_map>

#include "EntitySpawnSystem.h"
#include "Vec2.h"
#include "EntitySystem.h"
#include "Scene.h"

class EntityGenerator : public EntitySpawnSystem::Generator {
public:
    using SpawnFunction = std::function<EntityId (
            Scene&,
            const Vec2f&,
            NetworkDataComponent::Owner owner,
            const UUID& uuid)>;

    EntityGenerator() = default;
    virtual ~EntityGenerator() = default;
    virtual void RegisterSpawnFunctions() override;
    virtual EntityId SpawnEntity(
            const std::string& tag,
            Scene& targetScene,
            const Vec2f& targetPos,
            NetworkDataComponent::Owner owner,
            const UUID& uuid) override;

private:
    std::unordered_map<std::string, SpawnFunction> SpawnFunctions; 
};


//
//
