#include "EntityGenerator.h"
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
#include "HealthWatcherComponent.h"
#include "ControllerComponent.h"
#include "RespawnComponent.h"

#include "SpawnFunctionNotFoundException.h"

#include "CHKill.h"
#include <unordered_map>

using TeamId = TeamComponent::TeamId;

static void MakeLivingEntity(EntityId id, Vec2f pos, const Vec2f& colliderRes, float moveSpeed,
										TeamId team, Vec2f hurtboxOffset, Vec2f hurtboxRes, std::uint32_t health) {
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
    physicsComp->setCollidedWith(false);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(id);
	teamComp->teamId = team;

	auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(id);
	hurtComp->hurtbox.res = hurtboxRes;
	hurtComp->offset = hurtboxOffset;

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	healthComp->setHealth(health);

	auto topDownMoverComp = EntitySystem::GetComp<TopDownMoverComponent>(id);
	topDownMoverComp->setMoveSpeed(moveSpeed);
}

static void MakeHitboxEntity(EntityId id, Vec2f hitboxOffset, Vec2f hitboxRes, TeamId team, int damage) {
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
}

static void MakeBullet(EntityId id, Vec2f pos, Vec2f colliderRes, TeamId team, int damage) {
	MakeHitboxEntity(id, { 0, 0 }, colliderRes, team, damage);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id);

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->setRes(colliderRes);
	physicsComp->center(pos);
    physicsComp->loadCollisionHandler<CHKill>();
    physicsComp->setCollidedWith(false);
}

static void MakeGun(EntityId id, EntityId parent, const Vec2f& offset, float length) {
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

	auto gunFireComp = EntitySystem::GetComp<GunFireComponent>(id);
	gunFireComp->offset = length;
}

static std::vector<EntityId> SpawnBasicBullet(Scene& scene, const Vec2f& pos)
{
	auto entities = scene.addEntities(1);
	MakeBullet(entities[0], pos, { 4, 4 }, TeamId::neutral, 10);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entities[0]);
	auto lifeTimeComp = EntitySystem::GetComp<LifeTimeComponent>(entities[0]);
	lifeTimeComp->setRemainingLife(480);

	return entities;
}

static std::vector<EntityId> SpawnPlayer(Scene& scene, const Vec2f& pos) {
	auto entities = scene.addEntities(2);
	EntityId playerId = entities[0];
	MakeLivingEntity(playerId, pos, { 6, 4 }, 50.0f, TeamId::neutral, { -1, -11 }, { 8, 13 }, 100);
	EntitySystem::MakeComps<RespawnComponent>(1, &playerId);
	EntitySystem::GetComp<RespawnComponent>(playerId)->spawnPos = { 720.f / 4, 405.f / 4 };

	EntityId gunId = entities[1];
	MakeGun(gunId, playerId, { 3, -5 }, 13);
	return entities;
}

static std::vector<EntityId> SpawnEnemy(Scene& scene, const Vec2f& pos) {
	auto entities = scene.addEntities(1);
	EntityId enemyId = entities[0];
	MakeLivingEntity(enemyId, pos, { 6, 4 }, 50.0f, TeamId::enemy, { -1, -11 }, { 8, 13 }, 100);

	EntitySystem::MakeComps<BasicAttackComponent>(1, &enemyId);
	auto attackComp = EntitySystem::GetComp<BasicAttackComponent>(enemyId);
	attackComp->delay = 30;
	attackComp->duration = 95;
	attackComp->duration = 30;


	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnEntity(const std::string& tag, Scene& targetScene, const Vec2f& targetPos, NetworkOwnerComponent::Owner owner, bool shared) {
    if(EntityGenerator::SpawnFunctions.find(tag) == EntityGenerator::SpawnFunctions.end()) throw SpawnFunctionNotFoundException{tag};

    auto entities = EntityGenerator::SpawnFunctions.at(tag)(targetScene, targetPos);
    EntitySystem::MakeComps<NetworkOwnerComponent>(entities.size(), entities.data());
    for(const auto& entityId : entities) {
        EntitySystem::GetComp<NetworkOwnerComponent>(entityId)->owner = owner;
    }
    return entities;
}

void EntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::SpawnFunctions.insert(std::make_pair("player.basic", SpawnPlayer));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("enemy.basic", SpawnEnemy));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.basic", SpawnBasicBullet));
}
