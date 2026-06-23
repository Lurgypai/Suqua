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
#include "HandComponent.h"
#include "InventoryComponent.h"
#include "Shooty2NetworkDataFields.h"

#include "EntityAddNetwork.h"
#include "EntityAddPhysics.h"
#include "EntityAddBullet.h"
#include "EntityAddLiving.h"
#include "EntityAddHands.h"
#include "EntityAddDaemon.h"
#include "EntityAddAIGunner.h"
#include "EntityAddInventory.h"

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
        480,
        "CHKill"
    };
    EntityAddBullet(entity, bulletArgs);

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
        480,
        "CHKill"
    };
    EntityAddBullet(entity, bulletArgs);

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
        {0.f, -4.f},
        4.f,
        ControllerBits::BUTTON_5,
        ControllerBits::BUTTON_6
    };
    EntityAddHands(playerId, handsArgs);

	return playerId;
}

static EntityId SpawnDaemon(
    Scene& scene,
    const Vec2f& pos,
    Owner owner,
    const UUID& uuid) {
    auto entity = scene.addEntities(1)[0];
    EntityAddNetworkArgs netArgs{ uuid, owner };
    EntityAddNetwork(entity, netArgs);
    EntityAddPhysicsArgs physArgs{
        pos,
        Vec2f{4.f, 4.f},
        true,
        false,
        true
    };
    EntityAddPhysics(entity, physArgs);
    EntityAddHandsArgs handsArgs{
        Vec2f{0.f, 5.f},
        5.f,
        ControllerBits::BUTTON_7,
        ControllerBits::BUTTON_8
    };
    EntityAddHands(entity, handsArgs);
    EntityAddDaemonArgs daemonArgs{
        0.1f,
        Vec2f{-15, -15}
	};
	EntityAddDaemon(entity, daemonArgs);
    return entity;
}

static EntityId SpawnEnemy(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {
	auto entities = scene.addEntities(1);
	EntityId enemyId = entities[0];
    EntityAddNetworkArgs netArgs{ uuid, owner };
    EntityAddNetwork(enemyId, netArgs);
    EntityAddPhysicsArgs physArgs{
        pos,
        Vec2f{6.f, 4.f},
        true,
        false,
        false
    };
    EntityAddPhysics(enemyId, physArgs);
    EntityAddLivingArgs livingArgs {
        50.f,
        TeamId::enemy,
        {-1, -11},
        {8, 13},
        100,
        "deactivate"
    };
    EntityAddLiving(enemyId, livingArgs);
    EntityAddHandsArgs handsArgs{
        Vec2f{0.f, -7.f},
        4.f
    };
    EntityAddHands(enemyId, handsArgs);
    EntityAddAIGunnerArgs aiArgs{
        300.f,
        200.f
	};
	EntityAddAIGunner(enemyId, aiArgs);
    EntityAddInventoryArgs invArgs{
        {"item:gun:enemy_blast"},
        {1}
	};
	EntityAddInventory(enemyId, invArgs);

	return enemyId;
}
