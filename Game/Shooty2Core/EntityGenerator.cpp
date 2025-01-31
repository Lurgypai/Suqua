#include "EntityGenerator.h"
#include "AimToLStickComponent.h"
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
#include "ControllerComponent.h"
#include "RespawnComponent.h"
#include "AIGunnerComponent.h"
#include "PlayerSpawnComponent.h"

#include "SpawnFunctionNotFoundException.h"

#include "CHKill.h"
#include <unordered_map>
#include <vector>

using TeamId = TeamComponent::TeamId;
using Owner =  NetworkDataComponent::Owner;

static void MakeLivingEntity(
        EntityId id,
        const UUID& uuid,
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
        const UUID& uuid,
        Owner owner,
        Vec2f hitboxOffset,
        Vec2f hitboxRes,
        TeamId team,
        int damage ) {

    EntitySystem::MakeComps<NetworkDataComponent>(1, &id, uuid, owner);
	EntitySystem::MakeComps<TeamComponent>(1, &id, team);

	EntitySystem::MakeComps<HitboxComponent>(1, &id,
        hitboxOffset,
        hitboxRes,
        true ); // death on hit

	EntitySystem::MakeComps<DamageComponent>(1, &id);

	auto damageComp = EntitySystem::GetComp<DamageComponent>(id);
	damageComp->setDamageCalculator<BasicDamageCalculator>(damage);
}

static void MakeBullet(
        EntityId id,
        const UUID& uuid,
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

static EntityId SpawnBulletPlayerBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid)
{
	auto entities = scene.addEntities(1);
	MakeBullet(entities[0], uuid, owner, pos, { 4, 4 }, TeamId::player, 10);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entities[0], 480);

	return entities[0];
}

static EntityId SpawnBulletEnemyBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid)
{
	auto entities = scene.addEntities(1);
    MakeBullet(entities[0], uuid, owner, pos, { 4, 4 }, TeamId::enemy, 10);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entities[0], 480);

	return entities[0];
}

static EntityId SpawnPlayer(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {

	auto playerId = scene.addEntities(1)[0];
	MakeLivingEntity(
            playerId,
            uuid,
            owner,
            pos,
            { 6, 4 },
            50.0f,
            TeamId::player,
            { -1, -11 },
            { 8, 13 },
            100);
	EntitySystem::MakeComps<GunFireComponent>(1, &playerId,
            Vec2f{3.f, -5.f},
            13.f,
            "bullet.player.basic",
            /*
            3,
            0.2f,
            0.8f);
            */
            4,
            0.f,
            0.f,
            5,
            0.2f,
            55.f);

    EntitySystem::MakeComps<PlayerSpawnComponent>(1, &playerId);
	return playerId;
}

static EntityId SpawnEnemy(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {
	auto entities = scene.addEntities(1);
	EntityId enemyId = entities[0];
	MakeLivingEntity(enemyId, uuid, owner, pos, { 6, 4 }, 50.0f, TeamId::enemy, { -1, -11 }, { 8, 13 }, 100);

	EntitySystem::MakeComps<GunFireComponent>(1, &enemyId,
            Vec2f{},
            0.f,
            "bullet.enemy.basic",
            1,
            0.f,
            0.f,
            5,
            0.2f,
            30.f);

	EntitySystem::MakeComps<RespawnComponent>(1, &enemyId,
        Vec2f{ 720.f / 2, 405.f / 2 } );

    EntitySystem::MakeComps<AIGunnerComponent>(1, &enemyId,
            300.f,
            200.f );
	return enemyId;
}

EntityId EntityGenerator::SpawnEntity(
        const std::string& tag,
        Scene& targetScene,
        const Vec2f& targetPos,
        NetworkDataComponent::Owner owner,
        const UUID& uuid) {
    if(EntityGenerator::SpawnFunctions.find(tag) == EntityGenerator::SpawnFunctions.end()) throw SpawnFunctionNotFoundException{tag};

    auto entities = EntityGenerator::SpawnFunctions.at(tag)(
            targetScene,
            targetPos,
            owner,
            uuid);

    return entities;
}

void EntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::SpawnFunctions.insert(std::make_pair("player.basic", SpawnPlayer));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("enemy.basic", SpawnEnemy));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.player.basic", SpawnBulletPlayerBasic));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.enemy.basic", SpawnBulletEnemyBasic));
}
