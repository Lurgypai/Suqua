#include "EntityGenerator.h"
#include "AimToLStickComponent.h"
#include "TeamComponent.h"
#include "HurtboxComponent.h"
#include "PhysicsComponent.h"
#include "SideScrollMoverComponent.h"
#include "HealthComponent.h"
#include "NetworkOwnerComponent.h"
#include "LifeTimeComponent.h"
#include "RespawnComponent.h"
#include "BatSwingComponent.h"
#include "GolfSwingComponent.h"
#include "BounceCollisionHandler.h"
#include "WackableComponent.h"
#include "AIBall.h"
#include "SideScrollGFXComponent.h"
#include "ActiveEntityComponent.h"
#include "ActiveEntityZoneComponent.h"
#include "ControllerComponent.h"

using TeamId = TeamComponent::TeamId;

World* EntityGenerator::TargetLevel = nullptr;

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
    physicsComp->setCollidedWith(false);

	auto teamComp = EntitySystem::GetComp<TeamComponent>(id);
	teamComp->teamId = team;

	auto hurtComp = EntitySystem::GetComp<HurtboxComponent>(id);
	hurtComp->hurtbox.res = hurtboxRes;
	hurtComp->offset = hurtboxOffset;

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	healthComp->setHealth(health);
}

std::vector<EntityId> EntityGenerator::SpawnPlayer(Scene& scene, const Vec2f& pos) {
	auto entities = scene.addEntities(1);
	EntityId playerId = entities[0];
	MakeLivingEntity(playerId, pos, { 6, 12 }, 50.0f, TeamId::player, { -1, -11 }, { 8, 15 }, 100);

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

	auto* sideScrollComp = EntitySystem::GetComp<SideScrollMoverComponent>(playerId);
	sideScrollComp->accelGrounded = 2.0f;
	sideScrollComp->decel = 1.0f;
	sideScrollComp->accelAirborn = 0.5f;

	EntitySystem::MakeComps<SideScrollGFXComponent>(1, &playerId);
	EntitySystem::GetComp<SideScrollGFXComponent>(playerId)->loadSpriteSheet("player", "entities/player.json", Vec2f{ -29, -36 });

	EntitySystem::MakeComps<RespawnComponent>(1, &playerId);

    EntitySystem::MakeComps<ActiveEntityZoneComponent>(1, &playerId);
    auto spawnZone = EntitySystem::GetComp<ActiveEntityZoneComponent>(playerId);
    spawnZone->activeBox = AABB{{0, 0}, {480 * 2, 270 * 2}};
    spawnZone->inactiveBox = AABB{{0, 0}, {480 * 2 + 16, 270 * 2 + 16}};

	return entities;
}

std::vector<EntityId> EntityGenerator::SpawnEnemy(Scene& scene, const Vec2f& pos) {
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

    EntitySystem::MakeComps<WackableComponent>(1, &enemyId);
    EntitySystem::MakeComps<AIBallComponent>(1, &enemyId);

	EntitySystem::MakeComps<SideScrollGFXComponent>(1, &enemyId);
	EntitySystem::GetComp<SideScrollGFXComponent>(enemyId)->loadSpriteSheet("enemy:ball", "entities/ball.json", Vec2f{ -26, -36 });

    auto enemyAi = EntitySystem::GetComp<AIBallComponent>(enemyId);
    enemyAi->left = {-2.f, 5.f};
    enemyAi->right = {13.f, 5.f};
    enemyAi->downleft = {-2.f, 16.f};
    enemyAi->downright = {13.f, 16.f};
    enemyAi->level = TargetLevel;
	return entities;
}

void EntityGenerator::PlaceEnemy(Scene& scene, const Vec2f& pos) {
    auto id = SpawnEnemy(scene, pos)[0];
    EntitySystem::MakeComps<ActiveEntityComponent>(1, &id);
    auto activeComp = EntitySystem::GetComp<ActiveEntityComponent>(id);
    activeComp->spawnPos = pos;
    activeComp->deactivate();
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

std::vector<EntityId> EntityGenerator::SpawnEntity(Scene& scene, const std::string& tag, const Vec2f& pos) {
    auto spawnFuncIter = SpawnFunctions.find(tag);
    if(spawnFuncIter == SpawnFunctions.end()) return {};
    else return spawnFuncIter->second(scene, pos);
}

void EntityGenerator::PlaceEntity(Scene& scene, const std::string& tag, const Vec2f& pos) {
    auto spawnFuncIter = PlaceFunctions.find(tag);
    if(spawnFuncIter == PlaceFunctions.end()) return;
    else return spawnFuncIter->second(scene, pos);
}

EntityId EntityGenerator::PlaceEntities(Scene& scene, const World& world) {
    EntityId player = 0;

	for (auto& level : world.getLevels()) {
		for (auto& levelEntity : level.getEntities()) {
			if (levelEntity.id == "PlayerSpawn") {
				player = SpawnEntity(scene, "Player", levelEntity.pos)[0];
			}
            else {
                PlaceEntity(scene, levelEntity.id, levelEntity.pos);
            }
		}
	}
    return player;
}
