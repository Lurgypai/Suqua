#pragma once
#include <string>
#include <memory>
#include <vector>

#include <Vec2.h>
#include <EntitySystem.h>
#include <Scene.h>
#include "NetworkDataComponent.h"

class EntitySpawnSystem {
public:
    template<typename T, typename... Args>
    static void Init(Args... args);

    // generates an entity and sub entities, each with a unique uuid
    static std::vector<EntityId> SpawnEntity(
            const std::string& tag,
            Scene& scene,
            const Vec2f& pos,
            NetworkDataComponent::Owner owner,
            const std::vector<UUID>& uuids = std::vector<UUID>{});

    class Generator {
        friend EntitySpawnSystem;
    public:
        virtual ~Generator() = default;
    private:
        virtual void RegisterSpawnFunctions() = 0;
        virtual std::vector<EntityId> SpawnEntity(
                const std::string& tag,
                Scene& scene,
                const Vec2f& pos,
                NetworkDataComponent::Owner owner,
                const std::vector<UUID>& uuids = std::vector<UUID>{}) = 0;
    };
private:
    static std::unique_ptr<Generator> generator;    
};

template<typename T, typename... Args>
void EntitySpawnSystem::Init(Args... args) {
    generator = std::make_unique<T>(args...);
    generator->RegisterSpawnFunctions();
}
