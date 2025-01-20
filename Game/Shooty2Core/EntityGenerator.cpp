#include "EntityGenerator.h"
#include "AimToLStickComponent.h"
#include "ParentComponent.h"
#include "GunFireComponent.h"
#include "TeamComponent.h"
#include "HurtboxComponent.h"
#include "PhysicsComponent.h"
#include "TopDownMoverComponent.h"
#include "HealthComponent.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "HitboxComponent.h"
#include "LifeTimeComponent.h"
#include "HealthWatcherComponent.h"
#include "ControllerComponent.h"
#include "RespawnComponent.h"
#include "AIGunnerComponent.h"

#include "SpawnFunctionNotFoundException.h"

#include "CHKill.h"
#include <unordered_map>
#include <vector>

using TeamId = TeamComponent::TeamId;
using Owner =  NetworkDataComponent::Owner;

// Fill uuids if empty
static inline void FillUUIDS(std::vector<UUID>& uuids, int count) {
    if(!uuids.empty()) return;

    uuids.reserve(count);
    for(int i = 0; i != count; ++i) {
        uuids.push_back(UUID::GenerateUUID());
    }
}

static void MakeLivingEntity(
        EntityId id,
        UUID uuid,
        Owner owner,
        Vec2f pos,
        const Vec2f& colliderRes,
        float moveSpeed,
		TeamId team,
        Vec2f hurtboxOffset,
        Vec2f hurtboxRes,
        std::int32_t health ) {

    EntitySystem::MakeComps<NetworkDataComponent>(1, &id, uuid, owner);

	EntitySystem::MakeComps<PhysicsComponent>(1, &id,
            pos,
            colliderRes,
            true,
            false );

	EntitySystem::MakeComps<TeamComponent>(1, &id, team);

	EntitySystem::MakeComps<HurtboxComponent>(1, &id,
            hurtboxOffset,
            hurtboxRes
            );

	EntitySystem::MakeComps<ControllerComponent>(1, &id);

	EntitySystem::MakeComps<HealthComponent>(1, &id, health);

	EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
	EntitySystem::MakeComps<TopDownMoverComponent>(1, &id, moveSpeed);
}

static void MakeHitboxEntity(
        EntityId id,
        UUID uuid,
        Owner owner,
        Vec2f hitboxOffset,
        Vec2f hitboxRes,
        TeamId team,
        int damage ) {

    EntitySystem::MakeComps<NetworkDataComponent>(1, &id, uuid, owner);
	EntitySystem::MakeComps<TeamComponent>(1, &id, team);

	EntitySystem::MakeComps<HitboxComponent>(1, &id,
        hitboxOffset,
        hitboxRes );

	EntitySystem::MakeComps<DamageComponent>(1, &id);

	auto damageComp = EntitySystem::GetComp<DamageComponent>(id);
	damageComp->setDamageCalculator<BasicDamageCalculator>(damage);
}

static void MakeBullet(
        EntityId id,
        UUID uuid,
        Owner owner,
        Vec2f pos,
        Vec2f colliderRes,
        TeamId team,
        int damage) {
	MakeHitboxEntity(id, uuid, owner, { 0, 0 }, colliderRes, team, damage);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id,
            Vec2f{},
            colliderRes,
            true,
            false );

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->center(pos);
    physicsComp->loadCollisionHandler<CHKill>();
}

static void MakeGun(
        EntityId id,
        UUID uuid,
        Owner owner,
        EntityId parent,
        const Vec2f& offset,
        float length) {
    EntitySystem::MakeComps<NetworkDataComponent>(1, &id, uuid, owner);
	EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
	EntitySystem::MakeComps<ParentComponent>(1, &id,
            ParentComponent::OffsetMode::none,
            parent,
            offset,
            Vec2f{} );

	EntitySystem::MakeComps<GunFireComponent>(1, &id,
            length,
            "bullet.player.basic" );

	EntitySystem::MakeComps<HealthWatcherComponent>(1, &id,
            parent);
}

static std::vector<EntityId> SpawnBulletPlayerBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        std::vector<UUID> uuids)
{
	auto entities = scene.addEntities(1);
    FillUUIDS(uuids, entities.size());
	MakeBullet(entities[0], uuids[0], owner, pos, { 4, 4 }, TeamId::player, 10);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entities[0], 480);

	return entities;
}

static std::vector<EntityId> SpawnBulletEnemyBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        std::vector<UUID> uuids)
{
	auto entities = scene.addEntities(1);
    FillUUIDS(uuids, entities.size());
    MakeBullet(entities[0], uuids[0], owner, pos, { 4, 4 }, TeamId::enemy, 10);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entities[0], 480);

	return entities;
}

static std::vector<EntityId> SpawnPlayer(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        std::vector<UUID> uuids ) {

	auto entities = scene.addEntities(2);
    FillUUIDS(uuids, entities.size());

	EntityId playerId = entities[0];
	MakeLivingEntity(
            playerId,
            uuids[0],
            owner,
            pos,
            { 6, 4 },
            50.0f,
            TeamId::player,
            { -1, -11 },
            { 8, 13 },
            100);

	EntitySystem::MakeComps<RespawnComponent>(1, &playerId, 
        Vec2f{ 720.f / 4, 405.f / 4 } );

	EntityId gunId = entities[1];
	MakeGun(gunId, uuids[1], owner, playerId, { 3, -5 }, 13);
	return entities;
}

static std::vector<EntityId> SpawnEnemy(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        std::vector<UUID> uuids) {
	auto entities = scene.addEntities(1);
    FillUUIDS(uuids, entities.size());
	EntityId enemyId = entities[0];
	MakeLivingEntity(enemyId, uuids[0], owner, pos, { 6, 4 }, 50.0f, TeamId::enemy, { -1, -11 }, { 8, 13 }, 100);

	EntitySystem::MakeComps<GunFireComponent>(1, &enemyId,
            0.f,
            "bullet.enemy.basic" );

	EntitySystem::MakeComps<RespawnComponent>(1, &enemyId,
        Vec2f{ 720.f / 2, 405.f / 2 } );

    EntitySystem::MakeComps<AIGunnerComponent>(1, &enemyId,
            300,
            200 );
    /*
	EntitySystem::MakeComps<BasicAttackComponent>(1, &enemyId);
	auto attackComp = EntitySystem::GetComp<BasicAttackComponent>(enemyId);
	attackComp->delay = 30;
	attackComp->duration = 95;
	attackComp->duration = 30;
    */

	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnEntity(
        const std::string& tag,
        Scene& targetScene,
        const Vec2f& targetPos,
        NetworkDataComponent::Owner owner,
        const std::vector<UUID>& uuids) {
    if(EntityGenerator::SpawnFunctions.find(tag) == EntityGenerator::SpawnFunctions.end()) throw SpawnFunctionNotFoundException{tag};

    auto entities = EntityGenerator::SpawnFunctions.at(tag)(
            targetScene,
            targetPos,
            owner,
            uuids);

    return entities;
}

void EntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::SpawnFunctions.insert(std::make_pair("player.basic", SpawnPlayer));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("enemy.basic", SpawnEnemy));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.player.basic", SpawnBulletPlayerBasic));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.enemy.basic", SpawnBulletEnemyBasic));
}
