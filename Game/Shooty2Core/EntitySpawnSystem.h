#pragma once
#include <string>
#include <memory>
#include <vector>

#include <Vec2.h>
#include <EntitySystem.h>
#include <Scene.h>

#include "NetworkOwnerComponent.h"

class EntitySpawnSystem {
public:
    template<typename T, typename... Args>
    static void Init(Args... args);
    static std::vector<EntityId> SpawnEntity(const std::string& tag, Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner, bool shared);

    class Generator {
        friend EntitySpawnSystem;
    public:
        virtual ~Generator() = default;
    private:
        virtual void RegisterSpawnFunctions() = 0;
        virtual std::vector<EntityId> SpawnEntity(const std::string& tag, Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner, bool shared) = 0;
    };
private:
    static std::unique_ptr<Generator> generator;    
};

template<typename T, typename... Args>
void EntitySpawnSystem::Init(Args... args) {
    generator = std::make_unique<T>(args...);
    generator->RegisterSpawnFunctions();
}
