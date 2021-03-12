#include "BoxScene.h"
#include "Sprite.h"
#include "BoxComponent.h"
#include "NetworkDataComponent.h"
#include "PhysicsData.h"
#include "PhysicsComponent.h"
#include "PositionData.h"

BoxScene::BoxScene(SceneId id_)
	: Scene{id_}
{}

void BoxScene::load() {
	
	GLRenderer::LoadTexture("box.png", "box");

	auto boxId = addEntities(1)[0];
	EntitySystem::MakeComps<RenderComponent>(1, &boxId);
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(boxId);
	render->setDrawable<Sprite>("box");
	render->getDrawable<Sprite>()->setScale({3, 3});

	EntitySystem::MakeComps<BoxComponent>(1, &boxId);
	NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(boxId);
	data->get<float>(WEIGHT) = 4000.0f;
	data->get<bool>(COLLIDEABLE) = false;
	data->get<float>(X) = 480 / 2;
	data->get<float>(Y) = 640 / 2;
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(boxId);
	physics->setRes({ 75, 75 });
	EntitySystem::GetComp<RenderComponent>(boxId)->getDrawable<Sprite>()->setDepth(0.0);

	auto platformId = addEntities(1)[0];
	EntitySystem::MakeComps<PhysicsComponent>(1, &platformId);
	PhysicsComponent* platformPhysics = EntitySystem::GetComp<PhysicsComponent>(platformId);
	platformPhysics->setRes({1920, 4});
	NetworkDataComponent* platformData = EntitySystem::GetComp<NetworkDataComponent>(platformId);
	platformData->get<float>(X) = 0;
	platformData->get<float>(Y) = 1080 - 4;
	platformData->get<bool>(COLLIDEABLE) = true;
	platformData->get<bool>(FROZEN) = true;

	Camera cam{ {0,0}, {1920, 1080}, 1.0 };
	camId = GLRenderer::addCamera(cam);
}

void BoxScene::prePhysicsStep(double physics_step) {

}

void BoxScene::physicsStep(double physics_step) {
	physics.runPhysics(physics_step);
	for (auto&& box : EntitySystem::GetPool<BoxComponent>()) {
		box.update();
	}
}

void BoxScene::postPhysicsStep(double physics_step) {
}

void BoxScene::preRenderStep(RenderSystem& render) {
}

void BoxScene::renderStep(RenderSystem& render) {
	GLRenderer::Clear();
	drawScene(render);
	GLRenderer::Swap();
}

void BoxScene::postRenderStep(RenderSystem& render) {
}
