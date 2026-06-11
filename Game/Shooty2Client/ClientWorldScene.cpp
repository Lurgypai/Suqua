// #include <print>

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
#include "PHClientTeleport.h"
#include "PHClientDamage.h"

#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "HitboxComponent.h"
#include "TopDownMoverComponent.h"
#include "LifeTimeComponent.h"
#include "AimToLStickComponent.h"
#include "ParentComponent.h"
#include "HurtboxComponent.h"
#include "CharacterGFXComponent.h"
#include "PhysicsComponent.h"
#include "RespawnGFXComponent.h"
#include "AttackGFXComponent.h"
#include "ControllerComponent.h"
#include "TeleportZoneGFXComponent.h"
#include "DaemonGFXComponent.h"
#include "HandItemGFXComponent.h"

#include "../Shooty2Core/RespawnComponent.h"
#include "../Shooty2Core/HealthWatcherComponent.h"
#include "../Shooty2Core/OnHitComponent.h"
#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/AIGunnerComponent.h"
#include "../Shooty2Core/PlayerSpawnComponent.h"
#include "../Shooty2Core/InventoryComponent.h"
#include "../Shooty2Core/HandComponent.h"
#include "../Shooty2Core/DaemonComponent.h"

#include "../Shooty2Core/CommandRespawn.h"
#include "CommandItem.h"

// debug
// #include "PositionComponent.h"

ClientWorldScene::ClientWorldScene(SceneId id_, Scene::FlagType flags_, InputDeviceId input,
        ItemSystem& items_, HandItemGFXSystem& invItemGfx_) :
	Scene{ id_, flags_ },
	playerInput{ input },
    items{items_},
    invItemGfx{invItemGfx_}
{ }

void ClientWorldScene::load(Game& game)
{
    /* ------------------ NETWORKING ------------------ */
    game.loadPacketHandler<PHClientSpawnEntities>(Shooty2Packet::SpawnEntities, this);
    game.loadPacketHandler<PHClientState>(Packet::StateId, this);
    game.loadPacketHandler<PHClientDeadEntities>(Packet::DeadEntities);
    game.loadPacketHandler<PHClientTeleport>(Shooty2Packet::TeleportPlayer, myPlayerId);
    game.loadPacketHandler<PHClientDamage>(Shooty2Packet::Damage);

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
	GLRenderer::LoadTexture("player/player.png", "tex:hero");
	GLRenderer::LoadTexture("player/shadow.png", "tex:shadow");
	GLRenderer::LoadTexture("stranded/Enemies/Warrior/warrior.png", "tex:enemy:warrior");
    GLRenderer::LoadTexture("player/bullet.png", "tex:bullet:player");
	GLRenderer::LoadTexture("stranded/Tileset/custom_top_down.png", "tex:tileset");
    GLRenderer::LoadTexture("enemy/basic.png", "tex:enemy:basic");

    //particles
    GLRenderer::GenParticleType("part:exit", 1, ComputeShader{ "particles/test.vert" });

	/* ---------------- LOAD ENTITIES ----------------- */
    EntitySpawnSystem::Init<ClientEntityGenerator>(&game.host);
	// player
	static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput)).camera = camId;

	myPlayerId = EntitySpawnSystem::SpawnEntity("entity:player:basic", *this, { 720.f / 4, 405.f / 4 }, NetworkDataComponent::Owner::local_shared);
	addEntityInputs({ {myPlayerId, playerInput} });

	myDaemonId = EntitySpawnSystem::SpawnEntity("entity:player:daemon", *this, { 720.f / 4, 405.f / 4 }, NetworkDataComponent::Owner::local_shared);
	auto* daemonComp = EntitySystem::GetComp<DaemonComponent>(myDaemonId);
	daemonComp->hostEntity = myPlayerId;
	addEntityInputs({ {myDaemonId, playerInput} });

	// load level
    world = World{ "tex:tileset", "levels/test.ldtk" };
	world.load(*this);
    activeLevel = "Level_spawn";
    world.getLevel(activeLevel).activate();

    // prepare spawning
    auto* spawnComp = EntitySystem::GetComp<PlayerSpawnComponent>(myPlayerId);
    for(const auto& pair : world.getLevels()) {
        for(auto& entity : pair.second.getEntities()) {
            if(entity.id != "PlayerSpawn") continue;
            spawnComp->insertSpawnPos(pair.first, entity.pos + (entity.res / 2.f));
        }
    }
    auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
    plrPhysicsComp->teleport(spawnComp->getSpawnPos("Level_spawn"));

    // tell the server that we're ready
    ByteStream playerPacket;
    playerPacket << Shooty2Packet::AddPlayer;
    auto playerNdc = EntitySystem::GetComp<NetworkDataComponent>(myPlayerId);
    playerPacket << playerNdc->getUUID();
    game.host.bufferAllDataByChannel(0, playerPacket);

	/* ------------------------ OTHER DEBUG ------------------------ */
	// add test items to inventory
    auto* plrInvComp = EntitySystem::GetComp<InventoryComponent>(myPlayerId);
    plrInvComp->setItemCount("item:gun:basic", 1);
    plrInvComp->setItemCount("item:skill:dash", 1);
    plrInvComp->setItemCount("item:other:sprite", 4);

	/*-------------- COMMANDS ----------------*/
    DebugIO::getCommandManager().registerCommand<ExitCommand>();
    DebugIO::getCommandManager().registerCommand<CommandRespawn>(world);
    DebugIO::getCommandManager().registerCommand<CommandItem>(items, myPlayerId);


}

void ClientWorldScene::physicsStep(Game& game)
{
    Updater::UpdateOwned<AIGunnerComponent>(game.PHYSICS_STEP);
	Updater::UpdateOwned<TopDownMoverComponent>();
	Updater::UpdateOwned<ParentComponent>();
	Updater::UpdateOwned<AimToLStickComponent>();
	Updater::UpdateOwned<LifeTimeComponent>();
	Updater::UpdateOwned<HealthWatcherComponent>();
	Updater::UpdateOwned<RespawnComponent>();
	Updater::UpdateOwned<HandComponent>(*this, game.PHYSICS_STEP);
	Updater::UpdateOwned<DaemonComponent>();

    // combat is done entirely client side
	Updater::UpdateAll<HurtboxComponent>(); // Hurtboxes need to be moved to where the ndc says they are
	if(EntitySystem::Contains<HitboxComponent>()) Updater::UpdateAll<HitboxComponent>();

	combat.checkClientCollisions(&game.host);

	physics.runPhysicsOnOwned(game.PHYSICS_STEP);

	// update inputs for next frame
	auto& playerInputDevice = static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput));
	auto plrInvComp = EntitySystem::GetComp<HandComponent>(myPlayerId);
	playerInputDevice.entityPos = plrInvComp->getBodyPos();

    // load active level
	auto* plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
    auto newLevel = world.getActiveLevel(plrPhysicsComp->position());
    if(newLevel != nullptr && newLevel->getLevelId() != activeLevel) {
        world.getLevel(activeLevel).deactivate();
        newLevel->activate();
        activeLevel = newLevel->getLevelId();
    }

    // opening the menu
    auto cont = game.getInputDevice(playerInput).getControllerState();
    if(cont.toggled(ControllerBits::BUTTON_4) && cont[ControllerBits::BUTTON_4]) {
        // if our input is enabled, turn the menu on and disable input, else turn menu off and enable
        if(flags & Scene::Flag::input) game.sceneOn(menuScene);
        else game.sceneOff(menuScene);

        game.toggleSceneFlags(id, Scene::Flag::input);
    }
}

void ClientWorldScene::renderUpdateStep(Game& game)
{
	Updater::UpdateAll<CharacterGFXComponent>(game.PHYSICS_STEP * 1000);
	Updater::UpdateAll<HandItemGFXComponent>(invItemGfx);
    Updater::UpdateAll<OnHitComponent>();
    Updater::UpdateAll<RespawnGFXComponent>();
    Updater::UpdateAll<AttackGFXComponent>();
    Updater::UpdateAll<TeleportZoneGFXComponent>(game.PHYSICS_STEP * 1000);
	Updater::UpdateAll<DaemonGFXComponent>(game.PHYSICS_STEP);

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
}

void ClientWorldScene::renderStep(Game& game)
{
	/* ---------- DEBUG LINES ----------- */
	DebugIO::setLine(0, "Entity Count: " + std::to_string(EntitySystem::GetPool<EntityBaseComponent>().size()));
	DebugIO::setLine(1, "Player ID: " + std::to_string(myPlayerId));

	screenBuffer.bind();
	glClearColor(78.0f / 255, 59.0f / 255, 61.0f / 255, 1.0f);
	GLRenderer::Clear();
	drawScene(game.getRender());
    GLRenderer::DrawBufferedImages();
    GLRenderer::UpdateAndDrawParticles();

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
	
	
	// render inventory body pos
	// auto* plrInvComp = EntitySystem::GetComp<HandComponent>(myPlayerId);
	// auto* plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
	// RectDrawable bodyPosRect{ Color{1, 0, 0, 1}, false, -1.0f, AABB{plrInvComp->getBodyPos() - Vec2f{1.f, 1.f}, Vec2f{3.f, 3.f}}};
	// bodyPosRect.draw();
	// RectDrawable lhandRect{ Color{0, 1, 0, 1}, false, -1.0f, AABB{plrInvComp->getHandPos(0), Vec2f{1.f, 1.f}}};
	// lhandRect.draw();
	//RectDrawable rhandRect{ Color{0, 0, 1, 1}, false, -1.0f, AABB{plrInvComp->getHandPos(1) - Vec2f{1.f, 1.f}, Vec2f{3.f, 3.f}}};
	// rhandRect.draw();

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

// next steps
// make action slots rebindable DONE
// add basic "dash" item DONE
// setup left and right hand item offsets DONE
// setup item rendering DONE
// add daemon DONE
//		add entity that follows player DONE
//		add "stand here" command DONE
// add daemon rendering DONE
// add alternate hand position DONE
// add daemon switching sides DONE
// add item command
//		base command DONE
//		add errors, prevent crashes for invalid slot/item DONE
// move daemon to spawn interface DONE
// fix hand position on item switch DONE
// add flag to indicate when an item should target the "base" "doesTargetHost" DONE

// add inventory ui
//		render currently active items
//		add category strings to items
//		searchable structure

// add hand rendering
// check daemon networking
// add input buffering

// add skill overlap prevention DECIDED NOT TO DO FOR NOW
// key question, how/why does daemon skill overlap?
//		skill use puts the selected skill slot into a set of active skills
//		skills with same slot id can't be active concurrently
