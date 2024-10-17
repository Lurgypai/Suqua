#pragma once
#include <vector>

#include "Host.h"
#include "Vec2.h"
#include "EntitySystem.h"
#include "NetworkOwnerComponent.h"
#include "Scene.h"
#include "../Shooty2Core/EntityGenerator.h"

class ServerEntityGenerator : public EntityGenerator{
public:
    ServerEntityGenerator(Game* host);
    virtual ~ServerEntityGenerator() = default;
    virtual std::vector<EntityId> SpawnEntity(const std::string& tag, Scene& targetScene, const Vec2f& targetPos, NetworkOwnerComponent::Owner owner, bool shared) override;

private:
    Game* game;
};
