#include "ClientPlayingScene.h"
#include "Game.h"
#include "PlayerGFXComponent.h"
#include "ZombieGFXComponent.h"
#include "RectDrawable.h"

#include "PositionData.h"

using NDC = NetworkDataComponent;

ClientPlayingScene::ClientPlayingScene(SceneId id_, FlagType flags_, InputDeviceId keyboard_) :
	PlayingScene{id_, flags_},
	keyboard{keyboard_}
{
}

void ClientPlayingScene::load(Game& game) {
	PlayingScene::load(game);

	GLRenderer::LoadTexture("stabbyman.png", "stabbyman");
	GLRenderer::LoadTexture("zombie.png", "zombie");

	EntitySystem::MakeComps<RenderComponent>(1, &platformId);
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(platformId);
	render->loadDrawable<RectDrawable>();
	RectDrawable* rect = render->getDrawable<RectDrawable>();
	rect->filled = true;
	rect->c = { 1, 1, 1, 1 };
	rect->shape = { {0, 0}, {440, 12} };

	Camera cam{ { 0, 0,}, { 480, 270}, 1.0f};
	camId = GLRenderer::addCamera(cam);

	screenBuffer.bind();
	screenBuffer.addTexture2D(480, 270, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(480, 270);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();
}

void ClientPlayingScene::renderStep(Game& game) {
	screenBuffer.bind();
	GLRenderer::Clear();

	auto& cam = GLRenderer::getCamera(camId);
	NDC* data = EntitySystem::GetComp<NDC>(player);

	// don't move to the player before they've been initialized
	if (data) {
		cam.center(Vec2f{ data->get<float>(X), data->get<float>(Y) });
	}
	else {
		cam.center({ 0, 0 });
	}
	
	drawScene(game.getRender());

	static RectDrawable hitboxGFX{
	Color{1.0f, 0, 0, 1.0f},
	false,
	0.9,
	AABB{ {0, 0}, {10, 10} }
	};


	//for (auto& player : EntitySystem::GetPool<PlayerComponent>()) {
	//	if (EntitySystem::Contains<PhysicsComponent>()) {
	//		PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(player.getId());
	//		auto currAttack = player.getCurrAttack();
	//		if (currAttack) {
	//			auto currHitbox = currAttack->getCurrHitbox();
	//			if (currHitbox) {
	//				auto pos = currAttack->getPos();
	//				int dir = currAttack->getDir();
	//				for (const auto& aabb : currHitbox->hitboxes) {
	//					hitboxGFX.shape = {
	//						{ pos.x + aabb.pos.x * dir - (dir < 0 ? aabb.res.x : 0), pos.y + aabb.pos.y },
	//						{ aabb.res.x, aabb.res.y }
	//					};
	//					hitboxGFX.draw();
	//				}
	//			}
	//		}
	//	}
	//}

	//static RectDrawable physics{
	//	Color{0, 0, 1.0f, 1.0f},
	//	false,
	//	-0.9,
	//	AABB{}
	//};
	//for (auto& p : EntitySystem::GetPool<PhysicsComponent>()) {
	//	physics.shape = p.getCollider();
	//	physics.draw();
	//}

	Framebuffer::unbind();
	GLRenderer::DrawOverScreen(screenBuffer.getTexture(0).id);
}

void ClientPlayingScene::renderUpdateStep(Game& game) {
	// update graphics every tick to keep them perfectly in sync
	if (EntitySystem::Contains<PlayerGFXComponent>()) {
		for (auto& gfx : EntitySystem::GetPool<PlayerGFXComponent>()) {
			gfx.update(game.PHYSICS_STEP * 1000);
		}
	}

	if (EntitySystem::Contains<ZombieGFXComponent>()) {
		for (auto& gfx : EntitySystem::GetPool<ZombieGFXComponent>()) {
			gfx.update(game.PHYSICS_STEP * 1000);
		}
	}
}

void ClientPlayingScene::physicsStep(Game& game) {
	PlayingScene::physicsStep(game);
}

EntityId ClientPlayingScene::addPlayer() {
	EntityId playerId = PlayingScene::addPlayer();

	EntitySystem::MakeComps<PlayerGFXComponent>(1, &playerId);
	return playerId;
}

EntityId ClientPlayingScene::addZombie(Game& game, const Vec2f& targetPos) {
	EntityId playerId = PlayingScene::addZombie(game, targetPos);

	EntitySystem::MakeComps<ZombieGFXComponent>(1, &playerId);
	return playerId;
}

void ClientPlayingScene::addControl(EntityId id) {
	addEntityInputs({ {id, keyboard} });
	player = id;
}
