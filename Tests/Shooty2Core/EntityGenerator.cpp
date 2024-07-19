#include "EntityGenerator.h"
#include "PlayerComponent.h"
#include "AimToLStickComponent.h"
#include "ParentComponent.h"
#include "GunFireComponent.h"
#include "TeamComponent.h"
#include "HurtboxComponent.h"
#include "BasicAttackComponent.h"
#include "PhysicsComponent.h"
#include "TopDownMoverComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "NetworkOwnerComponent.h"
#include "HitboxComponent.h"
#include "LifeTimeComponent.h"
#include "RespawnComponent.h"
#include "HealthWatcherComponent.h"

using TeamId = TeamComponent::TeamId;

void EntityGenerator::MakeLivingEntity(EntityId id, Vec2f pos, const Vec2f& colliderRes, float moveSpeed,
										TeamId team, Vec2f hurtboxOffset, Vec2f hurtboxRes, std::uint32_t health,
										NetworkOwnerComponent::Owner owner) {
	EntitySystem::MakeComps<PhysicsComponent>(1, &id);
	EntitySystem::MakeComps<TeamComponent>(1, &id);
	EntitySystem::MakeComps<HurtboxComponent>(1, &id);
	EntitySystem::MakeComps<ControllerComponent>(1, &id);
	EntitySystem::MakeComps<NetworkOwnerComponent>(1, &id);
	EntitySystem::MakeComps<HealthComponent>(1, &id);
	EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
	EntitySystem::MakeComps<TopDownMoverComponent>(1, &id);

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->setRes(colliderRes);
	physicsComp->teleport(pos);
    physicsComp->setDoesCollide(true);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(id);
	teamComp->teamId = team;

	auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(id);
	hurtComp->hurtbox.res = hurtboxRes;
	hurtComp->offset = hurtboxOffset;

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	healthComp->setHealth(health);

	auto netOwnerComp = EntitySystem::GetComp<NetworkOwnerComponent>(id);
	netOwnerComp->owner = owner;

	auto topDownMoverComp = EntitySystem::GetComp<TopDownMoverComponent>(id);
	topDownMoverComp->setMoveSpeed(moveSpeed);
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
	auto entities = scene.addEntities(2);
	EntityId playerId = entities[0];
	MakeLivingEntity(playerId, pos, { 6, 4 }, 50.0f, TeamId::player, { -1, -11 }, { 8, 13 }, 100, owner);

	EntityId gunId = entities[1];
	MakeGun(gunId, playerId, { 3, -5 }, 13, owner);
	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnEnemy(Scene& scene, const Vec2f& pos, NetworkOwnerComponent::Owner owner) {
	auto entities = scene.addEntities(1);
	EntityId enemyId = entities[0];
	MakeLivingEntity(enemyId, pos, { 6, 4 }, 50.0f, TeamId::enemy, { -1, -11 }, { 8, 13 }, 100, owner);

	EntitySystem::MakeComps<BasicAttackComponent>(1, &enemyId);
	auto attackComp = EntitySystem::GetComp<BasicAttackComponent>(enemyId);
	attackComp->delay = 30;
	attackComp->duration = 95;
	attackComp->duration = 30;


	return entities;
}
