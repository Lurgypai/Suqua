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

#include "EntityMakeFunctions.h"
#include "Scene.h"
#include "LifeTimeComponent.h"
#include "GunFireComponent.h"
#include "PlayerSpawnComponent.h"
#include "AIGunnerComponent.h"

static EntityId SpawnTeleportZone(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {

    auto entities = scene.addEntities(1);
    MakePhysicsEntity(entities[0], uuid, owner, pos, {1.f, 1.f});

    return entities[0];
}

static EntityId SpawnTile(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid) {

    auto entities = scene.addEntities(1);
    MakePhysicsEntity(entities[0], uuid, owner, pos, {16, 16});

    return entities[0];
}

static EntityId SpawnBulletPlayerBasic(
        Scene& scene,
        const Vec2f& pos,
        Owner owner,
        const UUID& uuid)
{
	auto entities = scene.addEntities(1);
	MakeBullet(entities[0], uuid, owner, pos, { 3, 3 }, TeamId::player, 10);

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
            0,
            0.f,
            0.f,
            1,
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

    EntitySystem::MakeComps<AIGunnerComponent>(1, &enemyId,
            300.f,
            200.f );
	return enemyId;
}
