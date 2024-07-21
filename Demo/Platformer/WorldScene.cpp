#include "WorldScene.h"
#include "SideScrollGFXComponent.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "RectDrawable.h"
#include "DirectionComponent.h"
#include "Updater.h"
#include "PositionComponent.h"
#include "DebugIO.h"
#include "IDKeyboardMouse.h"
#include "AABB.h"
#include "ExitCommand.h"
#include "SideScrollMoverComponent.h"


constexpr unsigned int ScreenWidth = 1920 / 4;
constexpr unsigned int ScreenHeight = 1080 / 4;

WorldScene::WorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ },
	playerInput{ 0 }
{}

void WorldScene::load(Game& game)
{
    DebugIO::getCommandManager().registerCommand<ExitCommand>();

	/* ------------------ SET UP RENDERING -------------------- */
	// down scale buffer
	screenBuffer.bind();
	screenBuffer.addTexture2D(ScreenWidth, ScreenHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(ScreenWidth, ScreenHeight);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	// camera
	Camera cam{ {0, 0}, {ScreenWidth, ScreenHeight}, 1.0f };
	camId = GLRenderer::addCamera(cam);

	// textures
	GLRenderer::LoadTexture("stabbyman.png", "stabbyman");

	/* ---------------- LOAD ENTITIES ----------------- */
    auto entities = addEntities(2);
    // Player
    auto playerId = entities[0];

    EntitySystem::MakeComps<PhysicsComponent>(1, &playerId);
    auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(playerId);
    physicsComp->setRes( Vec2f{4, 4} );
    physicsComp->teleport( {ScreenWidth / 2, ScreenHeight / 2} );
    physicsComp->setCollidedWith(false);
	physicsComp->setWeight(5.0);
	physicsComp->setWeightless(false);
    
    EntitySystem::MakeComps<ControllerComponent>(1, &playerId);
    EntitySystem::MakeComps<DirectionComponent>(1, &playerId);

    EntitySystem::MakeComps<SideScrollMoverComponent>(1, &playerId); 
    auto sideScrollComp = EntitySystem::GetComp<SideScrollMoverComponent>(playerId);
	sideScrollComp->accelGrounded = 2.0f;
	sideScrollComp->decel = 1.0f;
	sideScrollComp->accelAirborn = 0.5f;

    EntitySystem::MakeComps<SideScrollGFXComponent>(1, &playerId);
    auto sideScrollGfx = EntitySystem::GetComp<SideScrollGFXComponent>(playerId);
    sideScrollGfx->loadSpriteSheet("stabbyman", "stabbyman.json", {-30, -44});

    auto input = game.loadInputDevice<IDKeyboardMouse>();
    static_cast<IDKeyboardMouse&>(game.getInputDevice(input)).camera = camId;
    addEntityInputs({ {playerId, input} });

    // Platform
    auto platformId = entities[1];
    EntitySystem::MakeComps<PhysicsComponent>(1, &platformId);
    auto platPhysics = EntitySystem::GetComp<PhysicsComponent>(platformId);
    platPhysics->setRes({ScreenWidth, 5});
    platPhysics->setDoesCollide(false);
    platPhysics->setCollidedWith(true);
    platPhysics->setWeightless(true);

    auto pos = EntitySystem::GetComp<PositionComponent>(platformId);
    pos->setPos({0, ScreenHeight - 5});

    EntitySystem::MakeComps<RenderComponent>(1, &platformId);
    auto renderComp = EntitySystem::GetComp<RenderComponent>(platformId);
    renderComp->loadDrawable<RectDrawable>( RectDrawable{
            Color{1, 1, 1, 1},
            true,
            0.f,
            AABB{{0, 0}, {ScreenWidth, 5}}
            });

}

void WorldScene::physicsStep(Game& game)
{
	Updater::UpdateAll<SideScrollMoverComponent>();
	physics.runPhysics(game.PHYSICS_STEP);
}

void WorldScene::renderUpdateStep(Game& game)
{
	Updater::UpdateAll<SideScrollGFXComponent>(game.PHYSICS_STEP * 1000);
}

void WorldScene::renderStep(Game& game)
{
	screenBuffer.bind();
	GLRenderer::Clear();
	drawScene(game.getRender());

    /*
    for(auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
        auto collider = physicsComp.getCollider(); 
        RectDrawable r{Color{1, 0, 1, 1}, false, 0.f, collider};
        r.draw();
    }
    */

	Framebuffer::unbind();
	GLRenderer::DrawOverScreen(screenBuffer.getTexture(0).id);
}

void WorldScene::unload(Game& game)
{
}

void WorldScene::onConnect(Game& game, PeerId connectingId)
{
}

void WorldScene::onDisconnect(Game& game, PeerId disconnectedPeer)
{
}
