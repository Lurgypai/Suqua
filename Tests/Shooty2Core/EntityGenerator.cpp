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

#include "RespawnComponent.h"
#include "HealthWatcherComponent.h"

void EntityGenerator::MakePlayer(EntityId playerId, EntityId gunId, TeamId team) {
	EntitySystem::MakeComps<PlayerComponent>(1, &playerId);
	EntitySystem::MakeComps<AimToLStickComponent>(1, &playerId);
	EntitySystem::MakeComps<TeamComponent>(1, &playerId);
	EntitySystem::MakeComps<HurtboxComponent>(1, &playerId);
	EntitySystem::MakeComps<DamageComponent>(1, &playerId);
	EntitySystem::MakeComps<NetworkOwnerComponent>(1, &playerId);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(playerId);
	teamComp->teamId = team;
	auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(playerId);
	hurtComp->hurtbox = AABB{ {0, 0}, {8, 13} };
	hurtComp->offset = { -1, -11 };
	
	EntitySystem::MakeComps<RespawnComponent>(1, &playerId);
	auto respawnComp = EntitySystem::GetComp<RespawnComponent>(playerId);
	respawnComp->spawnPos = { 100, 100 };


	EntitySystem::MakeComps<HealthComponent>(1, &playerId);
	auto healthComp = EntitySystem::GetComp<HealthComponent>(playerId);
	healthComp->setHealth(100);

	auto networkOwnerComp = EntitySystem::GetComp<NetworkOwnerComponent>(playerId);
	networkOwnerComp->owner = NetworkOwnerComponent::Owner::local;

	EntitySystem::MakeComps<AimToLStickComponent>(1, &gunId);
	EntitySystem::MakeComps<ParentComponent>(1, &gunId);
	EntitySystem::MakeComps<GunFireComponent>(1, &gunId);
	EntitySystem::MakeComps<HealthWatcherComponent>(1, &gunId);

	auto gunParentComp = EntitySystem::GetComp<ParentComponent>(gunId);
	gunParentComp->parentId = playerId;
	gunParentComp->baseOffset = { 0, -8 };

	auto healthWatcherComp = EntitySystem::GetComp<HealthWatcherComponent>(gunId);
	healthWatcherComp->parentId = playerId;
}

void EntityGenerator::MakeEnemy(EntityId enemyId, TeamId team) {
	EntitySystem::MakeComps<PhysicsComponent>(1, &enemyId);
	EntitySystem::MakeComps<ControllerComponent>(1, &enemyId);
	EntitySystem::MakeComps<TopDownMoverComponent>(1, &enemyId);
	EntitySystem::MakeComps<AimToLStickComponent>(1, &enemyId);
	EntitySystem::MakeComps<TeamComponent>(1, &enemyId);
	EntitySystem::MakeComps<HurtboxComponent>(1, &enemyId);
	EntitySystem::MakeComps<BasicAttackComponent>(1, &enemyId);
	EntitySystem::MakeComps<DamageComponent>(1, &enemyId);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(enemyId);
	teamComp->teamId = team;
	auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(enemyId);
	hurtComp->hurtbox = AABB{ {0, 0}, {8, 13} };
	hurtComp->offset = { -1, -11 };
	auto attackComp = EntitySystem::GetComp<BasicAttackComponent>(enemyId);
	attackComp->delay = 30;
	attackComp->duration = 95;
	attackComp->duration = 30;

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(enemyId);
	physicsComp->setRes({ 6, 4 });
	physicsComp->teleport({ 0, 0 });
	physicsComp->setCollideable(true);

	auto topDownMoverComp = EntitySystem::GetComp<TopDownMoverComponent>(enemyId);
	topDownMoverComp->setMoveSpeed(50.0);

	EntitySystem::MakeComps<HealthComponent>(1, &enemyId);
	auto healthComp = EntitySystem::GetComp<HealthComponent>(enemyId);
	healthComp->setHealth(100);

	EntitySystem::MakeComps<RespawnComponent>(1, &enemyId);
	auto respawnComp = EntitySystem::GetComp<RespawnComponent>(enemyId);
	respawnComp->spawnPos = { 720 / 2, 405 / 2 };
}
