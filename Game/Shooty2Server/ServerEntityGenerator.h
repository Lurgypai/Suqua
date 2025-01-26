#pragma once
#include <vector>

#include "Host.h"
#include "Vec2.h"
#include "EntitySystem.h"
#include "Scene.h"
#include "../Shooty2Core/EntityGenerator.h"

class ServerEntityGenerator : public EntityGenerator{
public:
    ServerEntityGenerator(Game* host);
    virtual ~ServerEntityGenerator() = default;
    virtual EntityId SpawnEntity(
            const std::string& tag,
            Scene& targetScene,
            const Vec2f& targetPos,
            NetworkDataComponent::Owner owner,
            const UUID& uuid = UUID::GenerateUUID()) override;

private:
    Game* game;
};
