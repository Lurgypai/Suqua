#pragma once
#include <unordered_map>

#include "Host.h"
#include "Vec2.h"
#include "EntitySystem.h"
#include "Scene.h"
#include "../Shooty2Core/EntityGenerator.h"

#include "EntityAddCharacterGFX.h"
#include "EntityAddHandGFX.h"
#include "EntityAddDaemonGFX.h"
#include "EntityAddRectGFX.h"
#include "EntityAddTeleportGFX.h"

using GFXEntityArgs = std::variant<
    EntityAddCharacterGFXArgs,
    EntityAddHandGFXArgs,
    EntityAddDaemonGFXArgs,
    EntityAddRectGFXArgs,
    EntityAddTeleportGFXArgs
>;

class ClientEntityGenerator : public EntityGenerator{
public:
    struct GFXEntity {
        std::string tag;
        std::vector<std::string> adds;
        std::vector<GFXEntityArgs> args;
    };
    using GFXFunction = std::function<void(EntityId)>;

    ClientEntityGenerator(const std::string& json, Host* host);
    virtual ~ClientEntityGenerator() = default;
    virtual EntityId SpawnEntity(
            const std::string& tag,
            Scene& targetScene,
            const Vec2f& targetPos,
            NetworkDataComponent::Owner owner,
            const Suqua::UUID& uuid = Suqua::UUID::GenerateUUID()) override;

private:
    std::unordered_map<std::string, GFXFunction> GFXFunctions; 
    std::unordered_map<std::string, GFXEntity> gfxEntities;
    Host* host;
};

// Wrapper that adds graphics to entity generator functions
