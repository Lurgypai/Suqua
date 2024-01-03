#pragma once
#include <vector>

#include "Vec2.h"
#include "EntitySystem.h"
#include "TeamComponent.h"
#include "NetworkOwnerComponent.h"
#include "Scene.h"
#include "Level.h"

namespace EntityGenerator {
    extern Level* TargetLevel;

	void MakeLivingEntity(EntityId id, Vec2f pos, const Vec2f& colliderRes, float moveSpeed,
							TeamComponent::TeamId team, Vec2f hurtboxOffset, Vec2f hurtboxRes, std::uint32_t health); // A thing with a hurtbox and health that can be controlled
	
	std::vector<EntityId> SpawnPlayer(Scene& scene, const Vec2f& pos); //based off of living entity
	std::vector<EntityId> SpawnEnemy(Scene& scene, const Vec2f& pos); // based off of living entity
    std::vector<EntityId> SpawnBall(Scene& scene, const Vec2f& pos);

    EntityId SpawnEntities(Scene& scene, const Level& level);
}
