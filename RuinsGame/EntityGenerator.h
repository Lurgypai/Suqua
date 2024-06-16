#pragma once
#include <vector>
#include <functional>
#include <unordered_map>

#include "Vec2.h"
#include "EntitySystem.h"
#include "TeamComponent.h"
#include "NetworkOwnerComponent.h"
#include "Scene.h"
#include "World.h"

//spawning refers to generating a single entity
//placing refers to spawning an entity and then tagging it with a positon to respawn

namespace EntityGenerator {
    extern World* TargetLevel;

	void MakeLivingEntity(EntityId id, Vec2f pos, const Vec2f& colliderRes, float moveSpeed,
							TeamComponent::TeamId team, Vec2f hurtboxOffset, Vec2f hurtboxRes, std::uint32_t health); // A thing with a hurtbox and health that can be controlled
	
	std::vector<EntityId> SpawnPlayer(Scene& scene, const Vec2f& pos); //based off of living entity
	std::vector<EntityId> SpawnEnemy(Scene& scene, const Vec2f& pos); //based off of living entity
    std::vector<EntityId> SpawnBall(Scene& scene, const Vec2f& pos);

    void PlaceEnemy(Scene& scene, const Vec2f& pos);

    EntityId PlaceEntities(Scene& scene, const World& level);

    std::vector<EntityId> SpawnEntity(Scene& scene, const std::string& tag, const Vec2f& pos);
    void PlaceEntity(Scene& scene, const std::string& tag, const Vec2f& pos);

    using SpawnFunction = std::function<std::vector<EntityId>(Scene&, const Vec2f&)>;
    const std::unordered_map<std::string, SpawnFunction> SpawnFunctions = {
        {"Ball", SpawnBall},
        {"EnemyBall", SpawnEnemy},
        {"Player", SpawnPlayer},
    };

    using PlaceFunction = std::function<void(Scene& scene, const Vec2f& pos)>;
    const std::unordered_map<std::string, PlaceFunction> PlaceFunctions = {
        {"EnemyBall", PlaceEnemy},
    };
}
