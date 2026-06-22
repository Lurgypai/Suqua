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
            const Suqua::UUID& uuid)>;

    EntityGenerator() = default;
    virtual ~EntityGenerator() = default;
    virtual void RegisterSpawnFunctions() override;
    virtual EntityId SpawnEntity(
            const std::string& tag,
            Scene& targetScene,
            const Vec2f& targetPos,
            NetworkDataComponent::Owner owner,
            const Suqua::UUID& uuid) override;

private:
    std::unordered_map<std::string, SpawnFunction> SpawnFunctions; 
};

/*
* spawn system changes
*   auto-generate entities from json
*       json includes a list of base entities
*       each base entity has corresponding make function and struct
*       store the string list of base entities and the vector of variadics
* steps
*   add string path to spawn system ui
*   header/cpp for each make function with the
*       struct
*       function
*   entity generator to have map of
*       struct Entity {
            std::string tag
            std::vector<std::string> subEntities;
*           std::vector<variadic> subEntityData
*       };
    spawn call
        lookup in map
        call corresponding make functions with their structs
        
*/
