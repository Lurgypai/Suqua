#include "ClientWorldScene.h"
#include "CharacterGFXComponent.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "AimToLStickComponent.h"
#include "../Shooty2Core/PlayerComponent.h"
#include "ParentComponent.h"
#include "GunGFXComponent.h"
#include "RectDrawable.h"
#include "DirectionComponent.h"
#include "HitboxComponent.h"
#include "TeamComponent.h"
#include "HurtboxComponent.h"
#include "Updater.h"
#include "PositionComponent.h"
#include "LifeTimeComponent.h"
#include "Level.h"
#include "../Shooty2Core/EntityGenerator.h"
#include "../Shooty2Core/GunFireComponent.h"
#include "TopDownMoverComponent.h"
#include "AITopDownBasic.h"
#include "DebugIO.h"
#include "IDKeyboardMouse.h"
#include "../Shooty2Core/BasicAttackComponent.h"
#include "HealthComponent.h"
#include "../Shooty2Core/RespawnComponent.h"
#include "../Shooty2Core/HealthWatcherComponent.h"
#include "../Shooty2Core/OnHitComponent.h"

ClientWorldScene::ClientWorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ },
	playerInput{ 0 }
{}

void ClientWorldScene::load(Game& game)
{
	/* ------------------ SET UP RENDERING -------------------- */
	// down scale buffer
	screenBuffer.bind();
	screenBuffer.addTexture2D(720, 405, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(720, 405);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	// camera
	Camera cam{ {8, 16}, {720, 405}, 1.0f };
	camId = GLRenderer::addCamera(cam);

	// textures
	GLRenderer::LoadTexture("stranded/Hero/Hero/green_hero.png", "hero");
	GLRenderer::LoadTexture("stranded/Enemies/Warrior/warrior.png", "enemy:warrior");
	GLRenderer::LoadTexture("stranded/Hero/Hero/green_gun.png", "gun");
	GLRenderer::LoadTexture("stranded/Tileset/custom_top_down.png", "tileset");

	/* ---------------- LOAD ENTITIES ----------------- */
	// player
	playerInput = game.loadInputDevice<IDKeyboardMouse>();
	static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput)).camera = camId;

	gunInput = game.loadInputDevice<IDKeyboardMouse>();
	static_cast<IDKeyboardMouse&>(game.getInputDevice(gunInput)).camera = camId;

	myPlayerId = addEntities(1)[0];
	myGunId = addEntities(1)[0];
	EntityGenerator::MakePlayer(myPlayerId, myGunId, 1);
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &myPlayerId);
	EntitySystem::GetComp<CharacterGFXComponent>(myPlayerId)->loadSpriteSheet("hero", "stranded/Hero/Hero/Hero.json", Vec2f{ -13, -24 });
	EntitySystem::GetComp<CharacterGFXComponent>(myPlayerId)->setHasUpDown(true);
	// EntitySystem::GetComp<CharacterGFXComponent>(myPlayerId)->loadSpriteSheet("enemy:warrior", "stranded/Enemies/Warrior/warrior.json", Vec2f{ -13, -24 });
	EntitySystem::MakeComps<OnHitComponent>(1, &myPlayerId);
	EntitySystem::MakeComps<GunGFXComponent>(1, &myGunId);
	addEntityInputs({ {myPlayerId, playerInput}, {myGunId, gunInput} });
	EntitySystem::GetComp<PhysicsComponent>(myPlayerId)->teleport({ 64, 64 });

	
	// dummy ai
	
	dummy = addEntities(1)[0];
	EntityGenerator::MakeEnemy(dummy, 2);
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &dummy);
	EntitySystem::GetComp<CharacterGFXComponent>(dummy)->loadSpriteSheet("enemy:warrior", "stranded/Enemies/Warrior/warrior.json", Vec2f{ -13, -24 });
	EntitySystem::MakeComps<OnHitComponent>(1, &dummy);
	dummyAI = game.loadInputDevice<AITopDownBasic>();
	addEntityInputs({ { dummy, dummyAI } });
	auto& ai = static_cast<AITopDownBasic&>(game.getInputDevice(dummyAI));
	ai.entityId = dummy;
	ai.setTargetTeams({ 1 });
	EntitySystem::GetComp<PhysicsComponent>(dummy)->teleport({ 720 / 2, 405 / 2 });
	
	

	// load level
	Level test{ "tileset", "levels/basic_test.ldtk" };
	test.load(*this);
}

void ClientWorldScene::physicsStep(Game& game)
{
	Updater::UpdateAll<TopDownMoverComponent>();
	Updater::UpdateAll<PlayerComponent>();
	Updater::UpdateAll<BasicAttackComponent>();
	Updater::UpdateAll<ParentComponent>();
	Updater::UpdateAll<AimToLStickComponent>();
	Updater::UpdateAll<GunFireComponent>(this);
	Updater::UpdateAll<HurtboxComponent>();
	Updater::UpdateAll<LifeTimeComponent>();
	Updater::UpdateAll<HealthWatcherComponent>();
	Updater::UpdateAll<RespawnComponent>();
	Updater::UpdateAll<OnHitComponent>();

	if(EntitySystem::Contains<HitboxComponent>()) Updater::UpdateAll<HitboxComponent>();

	combat.checkClientCollisions(&game.host);

	physics.runPhysics(game.PHYSICS_STEP);

	// update inputs for next frame
	auto& playerInputDevice = static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput));
	auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
	playerInputDevice.entityPos = plrPhysicsComp->center();
	auto& gunInputDevice = static_cast<IDKeyboardMouse&>(game.getInputDevice(gunInput));
	gunInputDevice.entityPos = plrPhysicsComp->center();
}

void ClientWorldScene::renderUpdateStep(Game& game)
{
	Updater::UpdateAll<CharacterGFXComponent>(game.PHYSICS_STEP * 1000);
	Updater::UpdateAll<GunGFXComponent>();
}

void ClientWorldScene::renderStep(Game& game)
{
	/* ---------- DEBUG LINES ----------- */
	DebugIO::setLine(0, "Player ID: " + std::to_string(myPlayerId));

	auto plrHealthComp = EntitySystem::GetComp<HealthComponent>(myPlayerId);
	DebugIO::setLine(1, "Player Health: " + std::to_string(plrHealthComp->getHealth()));


	screenBuffer.bind();
	glClearColor(78.0f / 255, 59.0f / 255, 61.0f / 255, 1.0f);
	GLRenderer::Clear();
	drawScene(game.getRender());

	/*
	if(EntitySystem::Contains<HitboxComponent>())
	for (auto& hitComp : EntitySystem::GetPool<HitboxComponent>()) {
		auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(hitComp.getId());
		if (!baseComp->isActive) continue;

		RectDrawable rect{ Color{1, 0, 0, 1}, false, -1.0f, hitComp.hitbox};
		rect.draw();
	}
	

	for (auto& hurtComp : EntitySystem::GetPool<HurtboxComponent>()) {
		RectDrawable rect{ Color{0, 0, 1, 1}, false, -1.0f, hurtComp.hurtbox };
		rect.draw();
	}
	*/

	/*
	for (auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
		RectDrawable rect{ Color{1, 0, 1, 1}, false, -1.0f, physicsComp.getCollider() };
		rect.draw();
	}
	*/

	/*
	auto& aIInput = static_cast<AITopDownBasic&>(game.getInputDevice(dummyAI));
	auto dummyPhysics = EntitySystem::GetComp<PhysicsComponent>(dummy);
	GLRenderer::DrawCircle(dummyPhysics->position(), -0.9, aIInput.followRadius, Color{
		aIInput.getState() == AITopDownBasic::AIState::attacking ? 1.0f : 0.0f,
		1.0f,
		aIInput.getState() == AITopDownBasic::AIState::following ? 1.0f : 0.0f,
		1.0f });
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