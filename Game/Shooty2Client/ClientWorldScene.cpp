#include "DebugIO.h"
#include "Game.h"
#include "Packet.h"
#include "Updater.h"
#include "ClientWorldScene.h"
#include "IDKeyboardMouse.h"
#include "World.h"
#include "ExitCommand.h"
#include "ClientEntityGenerator.h"

#include "PHClientState.h"
#include "PHClientSpawnEntities.h"
#include "PHClientAssignNetworkId.h"
#include "PHClientDeadEntities.h"

#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "HitboxComponent.h"
#include "TopDownMoverComponent.h"
#include "LifeTimeComponent.h"
#include "AimToLStickComponent.h"
#include "ParentComponent.h"
#include "GunGFXComponent.h"
#include "DirectionComponent.h"
#include "HurtboxComponent.h"
#include "CharacterGFXComponent.h"
#include "PhysicsComponent.h"
#include "RespawnGFXComponent.h"

#include "../Shooty2Core/PlayerComponent.h"
#include "../Shooty2Core/GunFireComponent.h"
#include "../Shooty2Core/BasicAttackComponent.h"
#include "../Shooty2Core/RespawnComponent.h"
#include "../Shooty2Core/HealthWatcherComponent.h"
#include "../Shooty2Core/OnHitComponent.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntitySpawnSystem.h"

ClientWorldScene::ClientWorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ },
	playerInput{ 0 }
{}

void ClientWorldScene::load(Game& game)
{
    DebugIO::getCommandManager().registerCommand<ExitCommand>();
    /* ------------------ NETWORKING ------------------ */
    game.loadPacketHandler<PHClientSpawnEntities>(Shooty2Packet::SpawnEntities, this);
    game.loadPacketHandler<PHClientState>(Packet::StateId, this);
    game.loadPacketHandler<PHClientAssignNetworkId>(Shooty2Packet::AssignNetworkId);
    game.loadPacketHandler<PHClientDeadEntities>(Packet::DeadEntities);

    game.host.tryConnect("127.0.0.1", 25565, 10);

	/* ------------------ SET UP RENDERING ------------------- */
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
    EntitySpawnSystem::Init<ClientEntityGenerator>(&game.host);
	// player
	playerInput = game.loadInputDevice<IDKeyboardMouse>();
	static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput)).camera = camId;

	auto playerAndGunId = EntitySpawnSystem::SpawnEntity("player.basic", *this, { 720.f / 4, 405.f / 4 }, NetworkOwnerComponent::Owner::local, true);
	myPlayerId = playerAndGunId[0];

	myGunId = playerAndGunId[1];
	addEntityInputs({ {myPlayerId, playerInput}, {myGunId, playerInput} });


    /*
	// dummy ai
	auto dummyEntities = EntityGenerator::SpawnEnemy(*this, { 720.f / 2, 405.f / 2 }, NetworkOwnerComponent::Owner::local);
	dummy = dummyEntities[0];
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &dummy);
	EntitySystem::GetComp<CharacterGFXComponent>(dummy)->loadSpriteSheet("enemy:warrior", "stranded/Enemies/Warrior/warrior.json", Vec2f{ -13, -24 });
	EntitySystem::MakeComps<OnHitComponent>(1, &dummy);

	EntitySystem::MakeComps<RespawnComponent>(1, &dummy);
	EntitySystem::GetComp<RespawnComponent>(dummy)->spawnPos = { 720.f / 2, 405.f / 2 };


	dummyAI = game.loadInputDevice<AITopDownBasic>();
	addEntityInputs({ { dummy, dummyAI } });
	auto& ai = static_cast<AITopDownBasic&>(game.getInputDevice(dummyAI));
	ai.entityId = dummy;
	ai.setTargetTeams({ TeamComponent::TeamId::player });
    */

	// load level
	World test{ "tileset", "levels/basic_test.ldtk" };
	test.load(*this);
    test.getLevels()[0].activate();
}

void ClientWorldScene::physicsStep(Game& game)
{
	Updater::UpdateOwned<TopDownMoverComponent>();
	Updater::UpdateOwned<PlayerComponent>();
	Updater::UpdateOwned<BasicAttackComponent>();
	Updater::UpdateOwned<ParentComponent>();
	Updater::UpdateOwned<AimToLStickComponent>();
	Updater::UpdateOwned<GunFireComponent>(this);
	Updater::UpdateOwned<LifeTimeComponent>();
	Updater::UpdateOwned<HealthWatcherComponent>();
	Updater::UpdateOwned<RespawnComponent>();

    // combat is done entirely client side
	Updater::UpdateAll<HurtboxComponent>(); // Hurtboxes need to be moved to where the ndc says they are
	if(EntitySystem::Contains<HitboxComponent>()) Updater::UpdateAll<HitboxComponent>();

	combat.checkClientCollisions(&game.host);

	physics.runPhysicsOnOwned(game.PHYSICS_STEP);

	// update inputs for next frame
	auto& playerInputDevice = static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput));
	auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
	playerInputDevice.entityPos = plrPhysicsComp->center();

    broadcastDeadEntities(game.host);
}

void ClientWorldScene::renderUpdateStep(Game& game)
{
	Updater::UpdateAll<CharacterGFXComponent>(game.PHYSICS_STEP * 1000);
	Updater::UpdateAll<GunGFXComponent>();
    Updater::UpdateAll<OnHitComponent>();
    Updater::UpdateAll<RespawnGFXComponent>();
}

void ClientWorldScene::renderStep(Game& game)
{
	/* ---------- DEBUG LINES ----------- */
	DebugIO::setLine(0, "Entity Count: " + std::to_string(EntitySystem::GetPool<EntityBaseComponent>().size()));
	DebugIO::setLine(1, "Player ID: " + std::to_string(myPlayerId));

	auto plrHealthComp = EntitySystem::GetComp<HealthComponent>(myPlayerId);
	DebugIO::setLine(2, "Player Health: " + std::to_string(plrHealthComp->getHealth()));


	screenBuffer.bind();
	glClearColor(78.0f / 255, 59.0f / 255, 61.0f / 255, 1.0f);
	GLRenderer::Clear();
	drawScene(game.getRender());
    GLRenderer::DrawBufferedImages();

	/*
	auto gunPos = EntitySystem::GetComp<PositionComponent>(myGunId);
	auto gunFireComp = EntitySystem::GetComp<GunFireComponent>(myGunId);
	auto firingPos = gunFireComp->getFiringPos();
	RectDrawable rect{ Color{1, 0, 0, 1}, true, -1.0f, {firingPos - Vec2f{0.5, 0.5}, {2, 2}}};
	rect.draw();
	rect = { Color{0.5f, 0, 0, 1}, true, -1.0f, {gunPos->getPos() - Vec2f{0.5, 0.5}, {2, 2}}};
	rect.draw();
	*/

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
