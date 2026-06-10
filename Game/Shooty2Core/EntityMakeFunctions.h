#ifndef ENTITYMAKEFUNCTIONS_H
#define ENTITYMAKEFUNCTIONS_H
#else
#error EntityMakeFunctions.h has already been included once. \
    This file should only be used once in EntitySpawnFunctions.h
#endif
/*
 * Tools for "makeing" entities
 * These functions form the basis for the spawn functions with actually put entities into the world
 * This header should not be used outside of the EntitySpawnFunctions.h file.
 */
#include "EntitySystem.h"
#include "NetworkDataComponent.h"
#include "UUID.h"
#include "TeamComponent.h"
#include "PhysicsComponent.h"
#include "AimToLStickComponent.h"
#include "HurtboxComponent.h"
#include "ControllerComponent.h"
#include "HealthComponent.h"
#include "TopDownMoverComponent.h"
#include "PositionComponent.h"
#include "HitboxComponent.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "CHKill.h"
#include "NetworkDataComponentDataFields.h"
#include "HandComponent.h"
#include "InventoryComponent.h"

using TeamId = TeamComponent::TeamId;
using Owner =  NetworkDataComponent::Owner;
using UUID = Suqua::UUID;

static void MakeNetworkEntity(EntityId id, const UUID& uuid, Owner owner) {
    EntitySystem::MakeComps<NetworkDataComponent>(1, &id, uuid, owner);
}

static void MakePhysicsEntity(
        EntityId id,
        // network entity
        const UUID& uuid,
        Owner owner,
        //moving entity
        Vec2f pos,
        const Vec2f& colliderRes) {
    //networking
    MakeNetworkEntity(id, uuid, owner);

    //physics
	EntitySystem::MakeComps<PhysicsComponent>(1, &id,
            pos,
            colliderRes,
            true,
            false);

    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    auto posComp = EntitySystem::GetComp<PositionComponent>(id);
    auto physComp = EntitySystem::GetComp<PhysicsComponent>(id);
    ndc->set(PositionData::X, posComp->pos.x);
    ndc->set(PositionData::Y, posComp->pos.y);

    ndc->set(PhysicsData::XVEL, physComp->vel.x);
    ndc->set(PhysicsData::XVEL, physComp->vel.y);
}

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
        Vec2f bodyOffset,
        float armLength,
        std::int32_t health) {
    MakePhysicsEntity(id, uuid, owner, pos, colliderRes);

	EntitySystem::MakeComps<TeamComponent>(1, &id, team);

	EntitySystem::MakeComps<HurtboxComponent>(1, &id,
            hurtboxOffset,
            hurtboxRes
            );

	EntitySystem::MakeComps<ControllerComponent>(1, &id);

	EntitySystem::MakeComps<HealthComponent>(1, &id, health);

	EntitySystem::MakeComps<AimToLStickComponent>(1, &id);
	EntitySystem::MakeComps<TopDownMoverComponent>(1, &id, moveSpeed);
    EntitySystem::MakeComps<InventoryComponent>(1, &id);
    EntitySystem::MakeComps<HandComponent>(1, &id, bodyOffset, armLength);

    auto healthComp = EntitySystem::GetComp<HealthComponent>(id);

    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    ndc->set(HealthData::HEALTH, healthComp->health);
}

static void MakeHitboxEntity(
        EntityId id,
        const UUID& uuid,
        Owner owner,
        Vec2f hitboxOffset,
        Vec2f hitboxRes,
        TeamId team,
        int damage ) {
    MakeNetworkEntity(id, uuid, owner);

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

    auto posComp = EntitySystem::GetComp<PositionComponent>(id);
    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    ndc->set(PositionData::X, posComp->pos.x);
    ndc->set(PositionData::Y, posComp->pos.y);

    ndc->set(PhysicsData::XVEL, physicsComp->vel.x);
    ndc->set(PhysicsData::XVEL, physicsComp->vel.y);
}
