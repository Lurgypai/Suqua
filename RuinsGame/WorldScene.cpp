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
#include "Level.h"
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


constexpr unsigned int ScreenWidth = 1920 / 4;
constexpr unsigned int ScreenHeight = 1080 / 4;

WorldScene::WorldScene(SceneId id_, Scene::FlagType flags_) :
	Scene{ id_, flags_ },
	playerInput{ 0 },
    level{}
{}

static InputDeviceId enemyInput;

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

	/* ---------------- LOAD ENTITIES ----------------- */
	// player

	auto playerId = EntityGenerator::SpawnPlayer(*this, { 720 / 4, 405 / 4 }, NetworkOwnerComponent::Owner::local);
	myPlayerId = playerId[0];
	EntitySystem::MakeComps<SideScrollGFXComponent>(1, &myPlayerId);
	EntitySystem::GetComp<SideScrollGFXComponent>(myPlayerId)->loadSpriteSheet("player", "entities/player.json", Vec2f{ -28, -36 });
	EntitySystem::MakeComps<OnHitComponent>(1, &myPlayerId);


	EntitySystem::MakeComps<RespawnComponent>(1, &myPlayerId);
	EntitySystem::GetComp<RespawnComponent>(myPlayerId)->spawnPos = { 720 / 4, 405 / 4 };

	playerInput = game.loadInputDevice<IDKeyboardMouse>();
	static_cast<IDKeyboardMouse&>(game.getInputDevice(playerInput)).camera = camId;
    

	addEntityInputs({ {myPlayerId, playerInput} });

	// load level
	level.load("tileset", "levels/test.ldtk", *this);

    // spawn dummy enemy
    EntityId enemyId = EntityGenerator::SpawnEnemy(*this, { 720 / 4 + 50, 405 / 4 }, NetworkOwnerComponent::Owner::local)[0];
	EntitySystem::MakeComps<SideScrollGFXComponent>(1, &enemyId);
	EntitySystem::GetComp<SideScrollGFXComponent>(enemyId)->loadSpriteSheet("enemy:ball", "entities/ball.json", Vec2f{ -26, -36 });

    auto enemyAi = EntitySystem::GetComp<AIBallComponent>(enemyId);
    enemyAi->left = {-2.f, 0.f};
    enemyAi->right = {13.f, 0.f};
    enemyAi->downleft = {-2.f, 16.f};
    enemyAi->downright = {13.f, 16.f};
    enemyAi->level = &level;
}

void WorldScene::physicsStep(Game& game)
{
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
	cam.center(plrPhysicsComp->center());
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
