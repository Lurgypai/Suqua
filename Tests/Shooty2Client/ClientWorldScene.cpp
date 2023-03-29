#include "ClientWorldScene.h"
#include "ClientPlayerComponent.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "AimToMouseComponent.h"
#include "../Shooty2Core/PlayerComponent.h"
#include "ParentComponent.h"
#include "GunGFXComponent.h"
#include "RectDrawable.h"
#include "DirectionComponent.h"
#include "../Shooty2Core/GunFireComponent.h"

ClientWorldScene::ClientWorldScene(SceneId id_, Scene::FlagType flags_, InputDeviceId playerInput_) :
	Scene{ id_, flags_ },
	playerInput{ playerInput_ }
{}

void ClientWorldScene::load(Game& game)
{
	screenBuffer.bind();
	screenBuffer.addTexture2D(480, 270, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(480, 270);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Camera cam{ {0, 0}, {480, 270}, 1.0f };
	camId = GLRenderer::addCamera(cam);

	GLRenderer::LoadTexture("stranded/Hero/Hero/green_hero.png", "hero");
	GLRenderer::LoadTexture("stranded/Hero/Hero/green_gun.png", "gun");

	myPlayerId = addEntities(1)[0];
	myGunId = addEntities(1)[0];

	EntitySystem::MakeComps<ClientPlayerComponent>(1, &myPlayerId);
	EntitySystem::MakeComps<AimToMouseComponent>(1, &myPlayerId);

	EntitySystem::MakeComps<AimToMouseComponent>(1, &myGunId);
	EntitySystem::MakeComps<ParentComponent>(1, &myGunId);
	EntitySystem::MakeComps<GunFireComponent>(1, &myGunId);
	EntitySystem::MakeComps<GunGFXComponent>(1, &myGunId);

	auto gunParentComp = EntitySystem::GetComp<ParentComponent>(myGunId);
	gunParentComp->parentId = myPlayerId;
	gunParentComp->offset = { 0, -8 };


	addEntityInputs({ {myPlayerId, playerInput}, {myGunId, playerInput} });
}

void ClientWorldScene::physicsStep(Game& game)
{
	auto playerComp = EntitySystem::GetComp<PlayerComponent>(myPlayerId);
	playerComp->update();

	auto gunParentComp = EntitySystem::GetComp<ParentComponent>(myGunId);
	gunParentComp->update();

	for (auto& aimComp : EntitySystem::GetPool<AimToMouseComponent>()) {
		aimComp.update();
	}

	auto gunFireComp = EntitySystem::GetComp<GunFireComponent>(myGunId);
	gunFireComp->update(*this);

	physics.runPhysics(game.PHYSICS_STEP);
}

void ClientWorldScene::renderUpdateStep(Game& game)
{
	auto clientPlayerComp = EntitySystem::GetComp<ClientPlayerComponent>(myPlayerId);
	clientPlayerComp->updateGFX();

	auto renderComp = EntitySystem::GetComp<RenderComponent>(myPlayerId);
	renderComp->getDrawable<AnimatedSprite>()->update(game.PHYSICS_STEP * 1000);

	auto gunGfxComp = EntitySystem::GetComp<GunGFXComponent>(myGunId);
	gunGfxComp->update();
}

void ClientWorldScene::renderStep(Game& game)
{
	screenBuffer.bind();
	glClearColor(78.0f / 255, 59.0f / 255, 61.0f / 255, 1.0f);
	GLRenderer::Clear();
	drawScene(game.getRender());

	/*
	for (auto& physicsComponent : EntitySystem::GetPool<PhysicsComponent>()) {
		RectDrawable rect{ Color{1, 0, 0, 1}, true, 0.0f, physicsComponent.getCollider()};
		rect.draw();
	}
	*/

	Framebuffer::unbind();
	GLRenderer::DrawOverScreen(screenBuffer.getTexture(0).id);
}

void ClientWorldScene::unload(Game& game)
{
}

void ClientWorldScene::onConnect(Game& game, PeerId connectingId)
{
}

void ClientWorldScene::onDisconnect(Game& game, PeerId disconnectedPeer)
{
}