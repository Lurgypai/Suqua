#pragma once
#include <unordered_map>
#include <vector>

#include "Host.h"
#include "Vec2.h"
#include "EntitySystem.h"
#include "Scene.h"
#include "../Shooty2Core/EntityGenerator.h"

class ClientEntityGenerator : public EntityGenerator{
public:
    using GFXFunction = std::function<void(const std::vector<EntityId>&)>;

    ClientEntityGenerator(Host* host);
    virtual ~ClientEntityGenerator() = default;
    virtual void RegisterSpawnFunctions() override;
    virtual std::vector<EntityId> SpawnEntity(
            const std::string& tag,
            Scene& targetScene,
            const Vec2f& targetPos,
            NetworkDataComponent::Owner owner,
            const std::vector<UUID>& uuids = std::vector<UUID>{}) override;

private:
    std::unordered_map<std::string, GFXFunction> GFXFunctions; 
    Host* host;
};

// Wrapper that adds graphics to entity generator functions
