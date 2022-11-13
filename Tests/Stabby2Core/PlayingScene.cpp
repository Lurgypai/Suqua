#include "PlayingScene.h"
#include "NetworkDataComponent.h"
#include "Game.h"
#include "PositionData.h"
#include "PlayerComponent.h"
#include "ZombieComponent.h"
#include "ZombieController.h"

using NDC = NetworkDataComponent;

PlayingScene::PlayingScene(SceneId id_, FlagType flags_) :
	Scene{id_, flags_},
	physics{}
{
}

PlayingScene::~PlayingScene()
{
}

void PlayingScene::load(Game& game) {
	platformId = addEntities(1)[0];
	EntitySystem::MakeComps<PhysicsComponent>(1, &platformId);
	PhysicsComponent* platformPhysics = EntitySystem::GetComp<PhysicsComponent>(platformId);
	platformPhysics->setRes({ 440, 12 });
	platformPhysics->setPos({ 20, 270 - 12 });
	platformPhysics->setCollideable(true);
	platformPhysics->setFrozen(true);
}

void PlayingScene::physicsStep(Game& game) {
	if (EntitySystem::Contains<PlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<PlayerComponent>()) {
			player.update(game);
		}
	}
	if (EntitySystem::Contains<ZombieComponent>()) {
		for (auto& player : EntitySystem::GetPool<ZombieComponent>()) {
			player.update(game);
		}
	}
	physics.runPhysics(game.PHYSICS_STEP);
	combat.update();
	respawn.update();
}

void PlayingScene::renderUpdateStep(Game& game)
{
}

void PlayingScene::renderStep(Game& game)
{
}

void PlayingScene::unload(Game& game)
{
}

void PlayingScene::onConnect(Game& game, PeerId connectingId)
{
}

void PlayingScene::onDisconnect(Game& game, PeerId disconnectedPeer)
{
}

EntityId PlayingScene::addPlayer() {
	EntityId playerId = addEntities(1).front();
	EntitySystem::MakeComps<PlayerComponent>(1, &playerId);
	return playerId;
}

EntityId PlayingScene::addZombie(Game& game, const Vec2f& targetPos) {
	EntityId playerId = addEntities(1).front();
	EntitySystem::MakeComps<ZombieComponent>(1, &playerId);

	auto input = game.loadInputDevice<ZombieController>(playerId);
	addEntityInputs({ {playerId, input} });

	auto* physics = EntitySystem::GetComp<PhysicsComponent>(playerId);
	physics->teleport(targetPos);

	return playerId;
}