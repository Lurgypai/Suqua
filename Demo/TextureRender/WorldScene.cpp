#include "WorldScene.h"
#include "PositionComponent.h"
#include "RenderComponent.h"
#include "Game.h"
#include "DirectionComponent.h"
#include "DebugIO.h"
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

    auto constexpr COUNT = 2;
    auto renderEntities = addEntities(COUNT);
    EntitySystem::MakeComps<RenderComponent>(COUNT, renderEntities.data());
    int index = 0;
    for(auto& id : renderEntities) {
        auto render = EntitySystem::GetComp<RenderComponent>(id);
        render->loadDrawable<Sprite>("none");
        auto pos = EntitySystem::GetComp<PositionComponent>(id);
        pos->setPos({0.f + 24 * index, 0.f});
        ++index;
    }
}

void WorldScene::physicsStep(Game& game)
{
}

void WorldScene::renderUpdateStep(Game& game)
{
}

void WorldScene::renderStep(Game& game)
{
    // bind the low resolution screen buffer
	screenBuffer.bind();
    //clear the screen buffer
	GLRenderer::Clear();
    
    // draw this scene using the render system
    drawScene(game.getRender());
    GLRenderer::DrawBufferedImages();
   

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
