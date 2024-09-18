#pragma once
#include <vector>

#include "Vec2.h"
#include "EntitySystem.h"
#include "TeamComponent.h"
#include "NetworkOwnerComponent.h"
#include "Scene.h"

namespace EntityGenerator {
	void MakeLivingEntity(EntityId id, Vec2f pos, const Vec2f& colliderRes, float moveSpeed,
							TeamComponent::TeamId team, Vec2f hurtboxOffset, Vec2f hurtboxRes, std::uint32_t health,
							NetworkOwnerComponent::Owner owner); // A thing with a hurtbox and health that can be controlled

	void MakeHitboxEntity(EntityId id, Vec2f hitboxOffset, Vec2f hitboxRes, TeamComponent::TeamId team, int damage, NetworkOwnerComponent::Owner owner); // a hitbox with a team and network owner and damage component
	void MakeBullet(EntityId id, Vec2f pos, Vec2f colliderRes, TeamComponent::TeamId team, int damage, NetworkOwnerComponent::Owner owner); // A hitbox entity with a physics component

	void MakeGun(EntityId id, EntityId parent, const Vec2f& offset, float length, NetworkOwnerComponent::Owner owner); // make a gun linked to an entity
	
	std::vector<EntityId> SpawnBasicBullet(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner);
	std::vector<EntityId> SpawnPlayer(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner); //based off of living entity
    std::vector<EntityId> SpawnPlayerPuppet(Scene& scene);
	std::vector<EntityId> SpawnEnemy(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner); // based off of living entity
}
