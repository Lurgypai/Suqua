#include "BoxScene.h"
#include "Sprite.h"
#include "BoxComponent.h"
#include "NetworkDataComponent.h"
#include "PhysicsData.h"
#include "PhysicsComponent.h"
#include "PositionData.h"
#include "Game.h"
#include "ControllerComponent.h"
BoxScene::BoxScene(SceneId id_, char flags_) :
	Scene{id_, flags_},
	boxId{0}
{}

void BoxScene::load(Game& game) {
	boxId = addEntities(1)[0];

	EntitySystem::MakeComps<BoxComponent>(1, &boxId);
	EntitySystem::MakeComps<ControllerComponent>(1, &boxId);
	NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(boxId);
	data->get<float>(WEIGHT) = 10.0f;
	data->get<bool>(COLLIDEABLE) = false;
	data->get<float>(X) = 480 / 2;
	data->get<float>(Y) = 640 / 2;
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(boxId);
	physics->setRes({ 75, 75 });

	auto platformId = addEntities(1)[0];
	EntitySystem::MakeComps<PhysicsComponent>(1, &platformId);
	PhysicsComponent* platformPhysics = EntitySystem::GetComp<PhysicsComponent>(platformId);
	platformPhysics->setRes({1920, 4});
	NetworkDataComponent* platformData = EntitySystem::GetComp<NetworkDataComponent>(platformId);
	platformData->get<float>(X) = 0;
	platformData->get<float>(Y) = 1080 - 4;
	platformData->get<bool>(COLLIDEABLE) = true;
	platformData->get<bool>(FROZEN) = true;
}

void BoxScene::prePhysicsStep(Game& game) {

}

void BoxScene::physicsStep(Game& game) {
	physics.runPhysics(game.PHYSICS_STEP);
	for (auto&& box : EntitySystem::GetPool<BoxComponent>()) {
		box.update();
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
