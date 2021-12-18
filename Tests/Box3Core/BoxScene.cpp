#include "BoxScene.h"
#include "Sprite.h"
#include "BoxComponent.h"
#include "NetworkDataComponent.h"
#include "PhysicsComponent.h"
#include "PositionData.h"
#include "Game.h"
#include "ControllerComponent.h"
#include "OnlineComponent.h"

BoxScene::BoxScene(SceneId id_, char flags_) :
	Scene{id_, flags_},
	boxes{}
{}

void BoxScene::load(Game& game) {
	auto platformId = addEntities(1)[0];
	EntitySystem::MakeComps<PhysicsComponent>(1, &platformId);
	PhysicsComponent* platformPhysics = EntitySystem::GetComp<PhysicsComponent>(platformId);
	platformPhysics->setRes({ 1920, 4 });
	platformPhysics->setPos({ 0, 1080 - 4 });
	platformPhysics->setCollideable(true);
	platformPhysics->setFrozen(true);
}

void BoxScene::prePhysicsStep(Game& game) {

}

void BoxScene::physicsStep(Game& game) {
	physics.runPhysics(game.PHYSICS_STEP);
	if (EntitySystem::Contains<BoxComponent>()) {
		for (auto&& box : EntitySystem::GetPool<BoxComponent>()) {
			box.update();
		}
	}
}

void BoxScene::postPhysicsStep(Game& game) {
}

void BoxScene::preRenderStep(Game& game) {
}

void BoxScene::renderStep(Game& game) {
}

void BoxScene::postRenderStep(Game& game) {
}

void BoxScene::unload(Game& game) {
}

void BoxScene::onConnect(Game& game, PeerId connetingId) {
}

void BoxScene::onDisconnect(Game& game, PeerId disconnectId)
{
}

EntityId BoxScene::addBox(Game& game, bool isClient, NetworkId targetId) {
	EntityId boxId = addEntities(1)[0];

	boxes.emplace_back(boxId);

	EntitySystem::MakeComps<BoxComponent>(1, &boxId);
	EntitySystem::MakeComps<ControllerComponent>(1, &boxId);
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(boxId);
	physics->setWeight(30.0f);
	physics->setCollideable(false);
	physics->setPos({ 1920 / 2, 1080 / 2 });
	physics->setRes({ 75, 75 });

	if (isClient) {
		game.online.registerOnlineComponent(boxId, targetId);
	}
	else {
		game.online.addOnlineComponent(boxId);
	}
	return boxId;
}
