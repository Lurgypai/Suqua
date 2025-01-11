#pragma once
#include <unordered_map>
#include <vector>

#include "EntitySpawnSystem.h"
#include "Vec2.h"
#include "EntitySystem.h"
#include "Scene.h"

class EntityGenerator : public EntitySpawnSystem::Generator {
public:
    using SpawnFunction = std::function<std::vector<EntityId>(Scene&, const Vec2f&)>;

    EntityGenerator() = default;
    virtual ~EntityGenerator() = default;
    virtual void RegisterSpawnFunctions() override;
    virtual std::vector<EntityId> SpawnEntity(const std::string& tag, Scene& targetScene, const Vec2f& targetPos, NetworkOwnerComponent::Owner owner, bool shared) override;

private:
    std::unordered_map<std::string, SpawnFunction> SpawnFunctions; 
};


//
//
