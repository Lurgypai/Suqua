#ifndef ENTITYSPAWNFUNCTIONS_H
#define ENTITYSPAWNFUNCTIONS_H
#else
#error EntitySpawnFucntions.h has already been included once. \
    This file should only be used once in EntityGenerator.cpp
#endif
/*
 * Tools for "spawning" entities, placing them into the world
 * This header should not be used outside of the EntityGenerator.cpp file.
 */

#include "Scene.h"
#include "LifeTimeComponent.h"
#include "PlayerSpawnComponent.h"
#include "AIGunnerComponent.h"
#include "DaemonComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponent.h"

#include "EntityAddNetwork.h"
#include "EntityAddPhysics.h"
#include "EntityAddBullet.h"
#include "EntityAddLiving.h"
#include "EntityAddHands.h"

using TeamId = TeamComponent::TeamId;
using Owner = NetworkDataComponent::Owner;
using UUID = Suqua::UUID;

static EntityId SpawnTeleportZone(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {

    auto entity = scene.addEntities(1)[0];
    EntityAddNetworkArgs netArgs{ uuid, owner };
    EntityAddNetwork(entity, netArgs);
    EntityAddPhysicsArgs args{
        pos,
        Vec2f{1.f, 1.f},
        false,
        false,
        true
    };
    EntityAddPhysics(entity, args);
    
    return entity;
}

static EntityId SpawnBulletPlayerBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid)
{
	auto entity = scene.addEntities(1)[0];
    EntityAddNetworkArgs netArgs{ uuid, owner };
    EntityAddNetwork(entity, netArgs);
    EntityAddPhysicsArgs physArgs {
        pos,
        Vec2f{3, 3},
        true,
        false,
        true
    };
    EntityAddPhysics(entity, physArgs);
    EntityAddBulletArgs bulletArgs{
        Vec2f{3, 3},
        TeamId::player,
        10,
        "CHKill"
    };
    EntityAddBullet(entity, bulletArgs);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entity, 480);

	return entity;
}

static EntityId SpawnBulletEnemyBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid)
{
	auto entity = scene.addEntities(1)[0];
    EntityAddNetworkArgs netArgs{ uuid, owner };
    EntityAddNetwork(entity, netArgs);
    EntityAddPhysicsArgs physArgs {
        pos,
        Vec2f{3, 3},
        true,
        false,
        true
    };
    EntityAddPhysics(entity, physArgs);
    EntityAddBulletArgs bulletArgs{
        Vec2f{3, 3},
        TeamId::enemy,
        10,
        "CHKill"
    };
    EntityAddBullet(entity, bulletArgs);

	EntitySystem::MakeComps<LifeTimeComponent>(1, &entity, 480);

	return entity;
}

static EntityId SpawnPlayer(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {

	auto playerId = scene.addEntities(1)[0];
    EntityAddNetworkArgs netArgs{ uuid, owner };
    EntityAddNetwork(playerId, netArgs);
    EntityAddPhysicsArgs physArgs {
        pos,
        Vec2f{6, 4},
        true,
        false,
        false
    };
    EntityAddPhysics(playerId, physArgs);
    EntityAddLivingArgs livingArgs {
        50.f,
        TeamId::player,
        {-1, -11},
        {8, 13},
        100,
        "none"
    };
    EntityAddLiving(playerId, livingArgs);
    EntityAddHandsArgs handsArgs {
        {0.f, 4.f},
        4.f
    };
    EntityAddHands(playerId, handsArgs);

    EntitySystem::MakeComps<PlayerSpawnComponent>(1, &playerId);

	return playerId;
}

static EntityId SpawnDaemon(
    Scene& scene,
    const Vec2f& pos,
    Owner owner,
    const UUID& uuid) {
    auto entity = scene.addEntities(1)[0];
    MakePhysicsEntity(entity, uuid, owner, pos, {4, 4});

	EntitySystem::MakeComps<ControllerComponent>(1, &entity);
	EntitySystem::MakeComps<DaemonComponent>(1, &entity, 0.1f, Vec2f{-15, -15});
	EntitySystem::MakeComps<HandComponent>(1, &entity, Vec2f{0.f, 5.f}, 5.f);
	auto* daemonHandComp = EntitySystem::GetComp<HandComponent>(entity);
    daemonHandComp->handFlags = { ControllerBits::BUTTON_7, ControllerBits::BUTTON_8 };
    return entity;
}

static EntityId SpawnEnemy(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {
	auto entities = scene.addEntities(1);
	EntityId enemyId = entities[0];
	MakeLivingEntity(enemyId, uuid, owner, pos, { 6, 4 }, 50.0f, TeamId::enemy, { -1, -11 }, { 8, 13 }, {0.f, -7}, 4.f, 100);
    EntitySystem::MakeComps<AIGunnerComponent>(1, &enemyId,
            300.f,
            200.f );

    auto* inv = EntitySystem::GetComp<InventoryComponent>(enemyId);
    inv->setItemCount("item:gun:enemy_blast", 1);

    auto* health = EntitySystem::GetComp<HealthComponent>(enemyId);
    auto cb = [](EntityId id) {
        auto* base = EntitySystem::GetComp<EntityBaseComponent>(id);
        base->isActive = false;
    };
    health->deathCallback = cb;

    auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(enemyId);
    auto* ai = EntitySystem::GetComp<AIGunnerComponent>(enemyId);
    ndc->set(AIData::AI_STATE, reinterpret_cast<std::uint8_t&>(ai->state));
	return enemyId;
}
