#include "EntityGenerator.h"
#include "PlayerComponent.h"
#include "AimToLStickComponent.h"
#include "ParentComponent.h"
#include "GunFireComponent.h"
#include "TeamComponent.h"
#include "HurtboxComponent.h"
#include "BasicAttackComponent.h"
#include "PhysicsComponent.h"
#include "SideScrollMoverComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "NetworkOwnerComponent.h"
#include "HitboxComponent.h"
#include "LifeTimeComponent.h"
#include "RespawnComponent.h"
#include "HealthWatcherComponent.h"
#include "BatSwingComponent.h"
#include "GolfSwingComponent.h"
#include "BounceCollisionHandler.h"

using TeamId = TeamComponent::TeamId;

void EntityGenerator::MakeLivingEntity(EntityId id, Vec2f pos, const Vec2f& colliderRes, float moveSpeed,
										TeamId team, Vec2f hurtboxOffset, Vec2f hurtboxRes, std::uint32_t health) {
	EntitySystem::MakeComps<PhysicsComponent>(1, &id);
	EntitySystem::MakeComps<TeamComponent>(1, &id);
	EntitySystem::MakeComps<HurtboxComponent>(1, &id);
	EntitySystem::MakeComps<ControllerComponent>(1, &id);
	EntitySystem::MakeComps<NetworkOwnerComponent>(1, &id);
	EntitySystem::MakeComps<HealthComponent>(1, &id);
	EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
	EntitySystem::MakeComps<SideScrollMoverComponent>(1, &id);

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->setRes(colliderRes);
	physicsComp->teleport(pos);
	physicsComp->setCollideable(true);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(id);
	teamComp->teamId = team;

	auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(id);
	hurtComp->hurtbox.res = hurtboxRes;
	hurtComp->offset = hurtboxOffset;

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	healthComp->setHealth(health);
}

void EntityGenerator::MakeHitboxEntity(EntityId id, Vec2f hitboxOffset, Vec2f hitboxRes, TeamId team, int damage, NetworkOwnerComponent::Owner owner) {
	EntitySystem::MakeComps<TeamComponent>(1, &id);
	EntitySystem::MakeComps<HitboxComponent>(1, &id);
	EntitySystem::MakeComps<DamageComponent>(1, &id);
	EntitySystem::MakeComps<NetworkOwnerComponent>(1, &id);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(id);
	teamComp->teamId = team;

	auto hitComp = EntitySystem::GetComp<HitboxComponent>(id);
	hitComp->hitbox.res = hitboxRes;
	hitComp->offset = hitboxOffset;

	auto damageComp = EntitySystem::GetComp<DamageComponent>(id);
	damageComp->setDamageCalculator<BasicDamageCalculator>(damage);

	auto ownerComp = EntitySystem::GetComp<NetworkOwnerComponent>(id);
	ownerComp->owner = owner;
}

void EntityGenerator::MakeBullet(EntityId id, Vec2f pos, Vec2f colliderRes, TeamId team, int damage, NetworkOwnerComponent::Owner owner) {
	MakeHitboxEntity(id, { 0, 0 }, colliderRes, team, damage, owner);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id);
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->setRes(colliderRes);
	physicsComp->center(pos);
}

void EntityGenerator::MakeGun(EntityId id, EntityId parent, const Vec2f& offset, float length, NetworkOwnerComponent::Owner owner) {
	EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
	EntitySystem::MakeComps<ParentComponent>(1, &id);
	EntitySystem::MakeComps<GunFireComponent>(1, &id);
	EntitySystem::MakeComps<HealthWatcherComponent>(1, &id);
	EntitySystem::MakeComps<NetworkOwnerComponent>(1, &id);

	auto parentComp = EntitySystem::GetComp<ParentComponent>(id);
	parentComp->parentId = parent;
	parentComp->baseOffset = offset;

	auto healthWatcherComp = EntitySystem::GetComp<HealthWatcherComponent>(id);
	healthWatcherComp->parentId = parent;

	auto networkOwnerComp = EntitySystem::GetComp<NetworkOwnerComponent>(id);
	networkOwnerComp->owner = owner;

	auto gunFireComp = EntitySystem::GetComp<GunFireComponent>(id);
	gunFireComp->offset = length;
}

std::vector<EntityId> EntityGenerator::SpawnBasicBullet(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner)
{
	auto entities = scene.addEntities(1);
	MakeBullet(entities[0], pos, { 4, 4 }, TeamId::neutral, 10, owner);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entities[0]);
	auto lifeTimeComp = EntitySystem::GetComp<LifeTimeComponent>(entities[0]);
	lifeTimeComp->setRemainingLife(480);

	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnPlayer(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner) {
	auto entities = scene.addEntities(1);
	EntityId playerId = entities[0];
	MakeLivingEntity(playerId, pos, { 8, 12 }, 50.0f, TeamId::player, { -1, -11 }, { 8, 15 }, 100);

	EntitySystem::MakeComps<BatSwingComponent>(1, &playerId);
	auto* batSwingComp = EntitySystem::GetComp<BatSwingComponent>(playerId);
	batSwingComp->delay = 36;
	batSwingComp->duration = 8;
	batSwingComp->lag = 12;

	EntitySystem::MakeComps<GolfSwingComponent>(1, &playerId);
	auto* golfSwingComp = EntitySystem::GetComp<GolfSwingComponent>(playerId);
	golfSwingComp->delay = 36;
	golfSwingComp->duration = 8;
	golfSwingComp->lag = 12;

	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(playerId);
	physicsComp->setWeight(5.0);
	physicsComp->setWeightless(false);

	// auto* sideScrollComp = EntitySystem::GetComp<SideScrollMoverComponent>(playerId);
	// sideScrollComp->moveSpeed = 10.0;
	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnEnemy(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner) {
	auto entities = scene.addEntities(1);
	EntityId enemyId = entities[0];
	MakeLivingEntity(enemyId, pos, { 12, 12 }, 50.0f, TeamId::enemy, { 0, 0 }, { 12, 12 }, 100);

	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(enemyId);
	physicsComp->setWeight(5.0);
	physicsComp->setWeightless(false);
	
	constexpr float MoveSpeed = 50.0f;
	auto* sideScrollComp = EntitySystem::GetComp<SideScrollMoverComponent>(enemyId);
	sideScrollComp->moveSpeed = MoveSpeed;
	sideScrollComp->accelAirborn = 0;
	sideScrollComp->accelGrounded = MoveSpeed;
	sideScrollComp->decel = MoveSpeed;

	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnBall(Scene& scene, const Vec2f& pos) {
    auto entities = scene.addEntities(1);
    EntityId ballId = entities[0];
    MakeLivingEntity(ballId, pos, {12, 12}, 50.0f, TeamId::player, { 0, 0 }, { 12, 12 }, 100); 

    
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(ballId);
	physicsComp->setWeight(5.0);
	physicsComp->setWeightless(false);
    physicsComp->loadCollisionHandler<BounceCollisionHandler>();

	constexpr float MoveSpeed = 50.0f;
	auto* sideScrollComp = EntitySystem::GetComp<SideScrollMoverComponent>(ballId);
	// sideScrollComp->moveSpeed = MoveSpeed;
	// sideScrollComp->accelAirborn = 0;
	// sideScrollComp->accelGrounded = MoveSpeed;
	sideScrollComp->decel = 0;

    return entities;
}
