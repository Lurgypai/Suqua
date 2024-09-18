#include "WorldScene.h"
#include "SideScrollGFXComponent.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "Game.h"
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
	Scene{ id_, flags_ }
{}

void WorldScene::load(Game& game)
{
    // load the basic "exit" command
    DebugIO::getCommandManager().registerCommand<ExitCommand>();

	/* ------------------ SET UP RENDERING -------------------- */
    // the world is rendered to a lower resolution buffer, which is then upscaled and rendered over the full screen
    // this creates a "true pixel" look
	screenBuffer.bind();
	screenBuffer.addTexture2D(ScreenWidth, ScreenHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(ScreenWidth, ScreenHeight);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	// camera
	Camera cam{ {0, 0}, {ScreenWidth, ScreenHeight}, 1.0f };
	camId = GLRenderer::addCamera(cam);

	// textures
    // textures are loaded using this call, taking the file path, and then a tag for identifiying the texture
    // when using the texture in-engine, we use the tag specified here to refer to it
	GLRenderer::LoadTexture("stabbyman.png", "stabbyman");

	/* ---------------- LOAD ENTITIES ----------------- */
    // scenes manage their own entities, so the addEntities function here is a member of the scene super class to generate an entity and add it to the scene.
    auto entities = addEntities(2);
    // Player
    auto playerId = entities[0];

    // Entities are represented by numerical ids with associated components
    // the physics component represents a collideable axis-aligned bounding-box, or AABB, that gets managed by the physics system
    // here we add one to the player, and set up its basic parameters
    EntitySystem::MakeComps<PhysicsComponent>(1, &playerId);
    auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(playerId);
    physicsComp->setRes( Vec2f{4, 4} );
    physicsComp->teleport( {ScreenWidth / 2, ScreenHeight / 2} );
    // things can't bump into this, but it can bump into other things
    physicsComp->setCollidedWith(false);
	physicsComp->setWeight(5.0);
	physicsComp->setWeightless(false);
    
    // The sidescrollmover defined below needs these components to function. 
    // The controller component represents an abstraction of an input device.
    // The inputs from the IDKeyboard mouse specified below are used to set the inputs to this controller
    // which the side scrollmover uses to move the player
    //
    // If you wanted to make an AI controlled player, you could initialize everything here, but simply add an additional 
    // "AI" component that sets the input state of the controller component
    // (and doesn't link an input device to the entity)
    EntitySystem::MakeComps<ControllerComponent>(1, &playerId);
    // this component just allows something to face left or right
    EntitySystem::MakeComps<DirectionComponent>(1, &playerId);

    // the side scroll mover uses inputs from the ControllerComponent to move the physics object around
    EntitySystem::MakeComps<SideScrollMoverComponent>(1, &playerId); 
    auto sideScrollComp = EntitySystem::GetComp<SideScrollMoverComponent>(playerId);
	sideScrollComp->accelGrounded = 2.0f;
	sideScrollComp->decel = 1.0f;
	sideScrollComp->accelAirborn = 0.5f;

    // the graphics component reads the state of the physics/mover components and uses them to render a sprite on top
    // sprites are drawn in Aseprite and exported as sprite sheets with json files defining the animations.
    // side sidescrollgfx relies on the loaded sprite having animations for "idle" and "left_right"
    // an optional "turn" animation can be included for when the sprite is turning around
    EntitySystem::MakeComps<SideScrollGFXComponent>(1, &playerId);
    auto sideScrollGfx = EntitySystem::GetComp<SideScrollGFXComponent>(playerId);
    sideScrollGfx->loadSpriteSheet("stabbyman", "stabbyman.json", {-30, -44});

    // create an Input Device (ID) for a Keyboard and Mouse and add it to the game
    // then associate that device with the player entity
    // this will make the scene set the ControllerComponent's state to the input devices state
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

    // The physics component uses a position component internally to track position
    // note that we're using the position component's "setPos()" function
    // instead of the physics component's "teleport()" function
    // "teleport()" moves a physics entity so that the bottom center of its collider is at the specified position
    // "setPos()" moves a physics entity so that the top left (its true position) of its collider is at the specified position
    auto pos = EntitySystem::GetComp<PositionComponent>(platformId);
    pos->setPos({0, ScreenHeight - 5});

    // Under the hood, the SideScrollGfxComponent uses a render component.
    // render components are components that take a drawable, and draw it to the screen
    // since we don't need anything fancy here, we just make a render component, and add a drawable rectangle to it
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
    // UpdateAll is a helper function that just calls the update function fo whatever component is specified for each component
	Updater::UpdateAll<SideScrollMoverComponent>();
	physics.runPhysics(game.PHYSICS_STEP);
    // this scene has an instance of the physics system which it uses to manage the physics components
}

void WorldScene::renderUpdateStep(Game& game)
{
    // we need to update the graphics so they match the current physics state
	Updater::UpdateAll<SideScrollGFXComponent>(game.PHYSICS_STEP * 1000);
}

void WorldScene::renderStep(Game& game)
{
    // bind the low resolution screen buffer
	screenBuffer.bind();
    //clear the screen
	GLRenderer::Clear();
    // draw this scene using the render system
	drawScene(game.getRender());
    GLRenderer::DrawBufferedImages();

    // this is some helper code that causes outlines of colliders to be drawn
    /*
    for(auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
        auto collider = physicsComp.getCollider(); 
        RectDrawable r{Color{1, 0, 1, 1}, false, 0.f, collider};
        r.draw();
    }
    */

    // unbind the low res screen buffer (rendering will now go to the default buffer, the window)
	Framebuffer::unbind();

    // this helper function just draws a texture over the entire window.
    // In this case we're drawing the low res buffer's texture
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
