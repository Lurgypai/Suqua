#include "PlayingScene.h"
#include "NetworkDataComponent.h"
#include "Game.h"
#include "PositionData.h"
#include "PlayerComponent.h"

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

void PlayingScene::prePhysicsStep(Game& game)
{
}

void PlayingScene::physicsStep(Game& game) {
	if (EntitySystem::Contains<PlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<PlayerComponent>()) {
			player.update(game);
		}
	}
	physics.runPhysics(game.PHYSICS_STEP);
	combat.update();
}

void PlayingScene::postPhysicsStep(Game& game)
{
}

void PlayingScene::preRenderStep(Game& game)
{
}

void PlayingScene::renderStep(Game& game)
{
}

void PlayingScene::postRenderStep(Game & game)
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