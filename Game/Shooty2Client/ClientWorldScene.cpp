#include "DebugIO.h"
#include "Game.h"
#include "Packet.h"
#include "Updater.h"
#include "ClientWorldScene.h"
#include "IDKeyboardMouse.h"
#include "ExitCommand.h"
#include "ClientEntityGenerator.h"

#include "PHClientState.h"
#include "PHClientSpawnEntities.h"
#include "PHClientDeadEntities.h"

#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "HitboxComponent.h"
#include "TopDownMoverComponent.h"
#include "LifeTimeComponent.h"
#include "AimToLStickComponent.h"
#include "ParentComponent.h"
#include "GunGFXComponent.h"
#include "HurtboxComponent.h"
#include "CharacterGFXComponent.h"
#include "PhysicsComponent.h"
#include "RespawnGFXComponent.h"
#include "AttackGFXComponent.h"
#include "ControllerComponent.h"

#include "../Shooty2Core/GunFireComponent.h"
#include "../Shooty2Core/RespawnComponent.h"
#include "../Shooty2Core/HealthWatcherComponent.h"
#include "../Shooty2Core/OnHitComponent.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntitySpawnSystem.h"
#include "../Shooty2Core/AIGunnerComponent.h"
#include "../Shooty2Core/PlayerSpawnComponent.h"

#include "../Shooty2Core/CommandRespawn.h"

ClientWorldScene::ClientWorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ },
	playerInput{ 0 }
{}

void ClientWorldScene::load(Game& game)
{
    DebugIO::getCommandManager().registerCommand<ExitCommand>();
    DebugIO::getCommandManager().registerCommand<CommandRespawn>(world);
    /* ------------------ NETWORKING ------------------ */
    game.loadPacketHandler<PHClientSpawnEntities>(Shooty2Packet::SpawnEntities, this);
    game.loadPacketHandler<PHClientState>(Packet::StateId, this);
    game.loadPacketHandler<PHClientDeadEntities>(Packet::DeadEntities);

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
    GLRenderer::LoadTexture("player/bullet.png", "bullet.player");
	GLRenderer::LoadTexture("stranded/Tileset/custom_top_down.png", "tileset");
    GLRenderer::LoadTexture("enemy/basic.png", "enemy:basic");

	/* ---------------- LOAD ENTITIES ----------------- */
    EntitySpawnSystem::Init<ClientEntityGenerator>(&game.host);
	// player
	playerInput = game.loadInputDevice<IDKeyboardMouse>();
	static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput)).camera = camId;

	myPlayerId = EntitySpawnSystem::SpawnEntity("player.basic", *this, { 720.f / 4, 405.f / 4 }, NetworkDataComponent::Owner::local_shared);
    //myGunId = playerAndGunId[1];
	addEntityInputs({ {myPlayerId, playerInput} });

    // EntitySpawnSystem::SpawnEntity("enemy.basic", *this, {720.f / 2, 405.f / 2}, NetworkDataComponent::Owner::local_only);

	// load level
    
    world = World{ "tileset", "levels/test.ldtk" };
	world.load(*this);
    world.getLevel("Level_spawn").activate();

    auto* spawnComp = EntitySystem::GetComp<PlayerSpawnComponent>(myPlayerId);
    for(const auto& pair : world.getLevels()) {
        for(auto& entity : pair.second.getEntities()) {
            if(entity.id != "PlayerSpawn") continue;
            spawnComp->insertSpawnPos(pair.first, entity.pos + (entity.res / 2.f));
        }
    }
    auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
    plrPhysicsComp->teleport(spawnComp->getSpawnPos("Level_spawn"));
}

void ClientWorldScene::physicsStep(Game& game)
{
    Updater::UpdateOwned<AIGunnerComponent>(game.PHYSICS_STEP);
	Updater::UpdateOwned<TopDownMoverComponent>();
	Updater::UpdateOwned<ParentComponent>();
	Updater::UpdateOwned<AimToLStickComponent>();
	Updater::UpdateOwned<GunFireComponent>(*this, game.PHYSICS_STEP);
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

    broadcastDeadEntities(game);
}

void ClientWorldScene::renderUpdateStep(Game& game)
{
	Updater::UpdateAll<CharacterGFXComponent>(game.PHYSICS_STEP * 1000);
	Updater::UpdateAll<GunGFXComponent>();
    Updater::UpdateAll<OnHitComponent>();
    Updater::UpdateAll<RespawnGFXComponent>();
    Updater::UpdateAll<AttackGFXComponent>();

	auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
    auto plrContComp = EntitySystem::GetComp<ControllerComponent>(myPlayerId);
	auto& cam = GLRenderer::getCamera(camId);

    auto plrPos = plrPhysicsComp->center();
    auto pointerWorldPos = GLRenderer::screenToWorld(plrContComp->getController().pointerPos, camId);
    auto delta = pointerWorldPos - plrPos;
    Vec2f targetPos = plrPos + (delta / 2.f);
    targetPos -= Vec2f{cam.res.x / 2.f, cam.res.y / 2.f };
	auto* level = world.getActiveLevel(plrPhysicsComp->center());

	if (level != nullptr) {
		auto& boundingBox = level->getBoundingBox();
		float leftOverlap = boundingBox.pos.x - targetPos.x;
		float rightOverlap = (boundingBox.pos.x + boundingBox.res.x) - (targetPos.x + cam.res.x);
		float topOverlap = boundingBox.pos.y - targetPos.y;
		float bottomOverlap = (boundingBox.pos.y + boundingBox.res.y) - (targetPos.y + cam.res.y);
		
		Vec2f offset{ 0,0 };
		if (leftOverlap > 0) offset.x = leftOverlap;
		if (rightOverlap < 0) offset.x = rightOverlap;
		if (topOverlap > 0) offset.y = topOverlap;
		if (bottomOverlap < 0) offset.y = bottomOverlap;

		targetPos += offset;
	}


	Vec2f distance = targetPos - cam.pos;
	if (distance.magn() < 1.0f) cam.pos = targetPos;
	else cam.pos += distance / 10.f;

    DebugIO::setLine(4, "Player Pos: " + std::to_string(plrPos.x) + ", " + std::to_string(plrPos.y));
    DebugIO::setLine(5, "Pointer Pos: " + std::to_string(pointerWorldPos.x) + ", " + std::to_string(pointerWorldPos.y));
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
