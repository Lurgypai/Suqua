#include "WorldScene.h"
#include "SideScrollGFXComponent.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "AimToLStickComponent.h"
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
#include "EntityGenerator.h"
#include "TopDownMoverComponent.h"
#include "AITopDownBasic.h"
#include "DebugIO.h"
#include "IDKeyboardMouse.h"
#include "HealthComponent.h"
#include "AABB.h"
#include "ExitCommand.h"

#include "BasicAttackComponent.h"
#include "RespawnComponent.h"
#include "HealthWatcherComponent.h"
#include "OnHitComponent.h"
#include "SideScrollMoverComponent.h"
#include "BatSwingComponent.h"
#include "GolfSwingComponent.h"
#include "FeelerComponent.h"
#include "AIBall.h"
#include "ActiveEntityComponent.h"
#include "ActiveEntityZoneComponent.h"
#include "EntityBaseComponent.h"


constexpr unsigned int ScreenWidth = 1920 / 4;
constexpr unsigned int ScreenHeight = 1080 / 4;

WorldScene::WorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ },
	playerInput{ 0 },
    world{}
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
	GLRenderer::LoadTexture("entities/stabbyman.png", "stabbyman");
	GLRenderer::LoadTexture("entities/player.png", "player");
	GLRenderer::LoadTexture("entities/ball.png", "enemy:ball");
	GLRenderer::LoadTexture("levels/tileset.png", "tileset");
    GLRenderer::LoadTexture("entities/bouncing_ball.png", "bounce_ball");

	/* ---------------- LEVEL ----------------- */
	// load level
	world.load("tileset", "levels/test.ldtk", *this);
    EntityGenerator::TargetLevel = &world;

	/* ---------------- LOAD ENTITIES ----------------- */
	// player
    myPlayerId = EntityGenerator::PlaceEntities(*this, world);

	playerInput = game.loadInputDevice<IDKeyboardMouse>();
	static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput)).camera = camId;
	addEntityInputs({ {myPlayerId, playerInput} });
}

void WorldScene::physicsStep(Game& game)
{
    for(auto& level : world.getLevels()) {
        auto& bounding = level.getBoundingBox();
        auto playerActiveZone = EntitySystem::GetComp<ActiveEntityZoneComponent>(myPlayerId);
        if(!level.isActive() && bounding.intersects(playerActiveZone->inactiveBox)) level.activate();
        else if (level.isActive() && !bounding.intersects(playerActiveZone->inactiveBox)) level.deactivate();
    }

	Updater::UpdateAll<TopDownMoverComponent>();
	Updater::UpdateAll<BasicAttackComponent>();
	Updater::UpdateAll<HurtboxComponent>();
	Updater::UpdateAll<HealthWatcherComponent>();
	Updater::UpdateAll<RespawnComponent>();
	// Updater::UpdateAll<OnHitComponent>();
	Updater::UpdateAll<SideScrollMoverComponent>();
	Updater::UpdateAll<BatSwingComponent>();
	Updater::UpdateAll<GolfSwingComponent>();
	Updater::UpdateAll<ParentComponent>();
	Updater::UpdateAll<HitboxComponent>();
    Updater::UpdateAll<AIBallComponent>();
    Updater::UpdateAll<ActiveEntityComponent>();
    Updater::UpdateAll<ActiveEntityZoneComponent>();


	//combat.checkClientCollisions(&game.host);

	physics.runPhysics(game.PHYSICS_STEP);
    wackable.update(*this);

	// update inputs for next frame
    /*
	auto& playerInputDevice = static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput));
	auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
	playerInputDevice.entityPos = plrPhysicsComp->center();
    */
}

void WorldScene::renderUpdateStep(Game& game)
{
	Updater::UpdateAll<SideScrollGFXComponent>(game.PHYSICS_STEP * 1000);
	Updater::UpdateAll<GunGFXComponent>();

	auto plrPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(myPlayerId);
	auto& cam = GLRenderer::getCamera(camId);

	Vec2f targetPos = plrPhysicsComp->center() - Vec2f{ cam.res.x / 2.f, cam.res.y / 2.f };
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

void WorldScene::renderStep(Game& game)
{
	/* ---------- DEBUG LINES ----------- */
	DebugIO::setLine(0, "Entity Count: " + std::to_string(EntitySystem::GetPool<EntityBaseComponent>().size()));
	DebugIO::setLine(1, "Player ID: " + std::to_string(myPlayerId));

	auto plrHealthComp = EntitySystem::GetComp<HealthComponent>(myPlayerId);
	DebugIO::setLine(2, "Player Health: " + std::to_string(plrHealthComp->getHealth()));

	auto plrDirComp = EntitySystem::GetComp<DirectionComponent>(myPlayerId);
	DebugIO::setLine(3, "Player CDir: " + std::to_string(plrDirComp->getCardinalDir()));

	Vec2i mousePos;
	auto mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);

	auto worldPos = GLRenderer::screenToWorld(mousePos, camId);
	DebugIO::setLine(4, "Mouse Pos: " + std::to_string(worldPos.x) + ", " + std::to_string(worldPos.y));


	screenBuffer.bind();
	// glClearColor(78.0f / 255, 59.0f / 255, 61.0f / 255, 1.0f);
	GLRenderer::Clear();
	drawScene(game.getRender());
    GLRenderer::DrawBufferedImages();
	//GLRenderer::setCamera(camId);
	
    /*
	if(EntitySystem::Contains<HitboxComponent>())
	for (auto& hitComp : EntitySystem::GetPool<HitboxComponent>()) {
		auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(hitComp.getId());
		if (!baseComp->isActive) continue;

		RectDrawable rect{ Color{1, 0, 0, 1}, false, -1.0f, hitComp.hitbox};
		rect.draw();
	}
    */
	
    /*
    auto& aIInput = static_cast<AIBall&>(game.getInputDevice(enemyInput));
    Vec2f startPos = EntitySystem::GetComp<PositionComponent>(aIInput.entityId)->getPos();
    Vec2f aILeft = startPos + aIInput.left;
    Vec2f aIRight = startPos + aIInput.right;
    Vec2f aIDownLeft = startPos + aIInput.downleft;
    Vec2f aIDownRight = startPos + aIInput.downright;

    Color leftColor{0.0, 1.0, 0.0, 1.0};
    if(level.hasTile(aILeft)) leftColor.r = 1.0;
    Color rightColor{0.0, 1.0, 0.0, 1.0};
    if(level.hasTile(aIRight)) rightColor.r = 1.0;
    Color downLeftColor{0.0, 1.0, 0.0, 1.0};
    if(!level.hasTile(aIDownLeft)) downLeftColor.r = 1.0;
    Color downRightColor{0.0, 1.0, 0.0, 1.0};
    if(!level.hasTile(aIDownRight)) downRightColor.r = 1.0;
    GLRenderer::DrawPrimitive(Primitive{{startPos, aILeft}, -0.5, leftColor});
    GLRenderer::DrawPrimitive(Primitive{{startPos, aIRight}, -0.5, rightColor});
    GLRenderer::DrawPrimitive(Primitive{{startPos, aIDownLeft}, -0.5, downLeftColor});
    GLRenderer::DrawPrimitive(Primitive{{startPos, aIDownRight}, -0.5, downRightColor});
    */

	//for (auto& hurtComp : EntitySystem::GetPool<HurtboxComponent>()) {
	//	RectDrawable rect{ Color{0, 0, 1, 1}, false, -1.0f, hurtComp.hurtbox };
	//	rect.draw();
	//}

    /*
	for (auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
		RectDrawable rect{ Color{0, 0, 1, 1}, false, -1.0f, physicsComp.getCollider() };
		rect.draw();
	}
    */

    for (auto& activeComp : EntitySystem::GetPool<ActiveEntityZoneComponent>()) {
		RectDrawable rect{ Color{0, 0, 1, 1}, false, -1.0f, activeComp.activeBox};
		RectDrawable rect2{ Color{1, 0, 0, 1}, false, -1.0f, activeComp.inactiveBox};
		rect.draw();
		rect2.draw();
    }

	
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
