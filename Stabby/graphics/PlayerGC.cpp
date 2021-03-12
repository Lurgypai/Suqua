#include "PlayerGC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "HeadParticleLC.h"
#include "Color.h"
#include "TextDrawable.h"
#include "EntityBaseComponent.h"
#include "DirectionData.h"
#include "PositionData.h"
#include "combat.h"

using NDC = NetworkDataComponent;

PlayerGC::PlayerGC(EntityId id_) :
	id{ id_ },
	shouldSpawnHead{ true },
	prevXVel{ 0 },
	prevAttack{ 0 },
	attackAnimations{},
	currAttackTag{},
	animationFrozen{ false },
	prevStunFrame{0},
	wasStunned{false}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
		}
	}
}

void PlayerGC::loadAnimations(const WeaponManager& weapons) {
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite& animSprite_ = *render->getDrawable<AnimatedSprite>();


	//keep in mind graphics gale starts at frame 1, not 0
	animSprite_.addAnimation(idle, 0, 1);
	animSprite_.addAnimation(walking, 1, 17);
	animSprite_.addAnimation(roll, 30, 37);
	animSprite_.addAnimation(stun, 39, 40);
	animSprite_.addAnimation(dead, 40, 52);
	animSprite_.addAnimation(climb, 52, 54);
	animSprite_.addAnimation(heal, 54, 64);
	animSprite_.addAnimation(crouch, 54, 57);

	//ClientPlayerLC* playerLogic = EntitySystem::GetComp<ClientPlayerLC>(id);
	//Vec2f res = playerLogic->getRes();
	Vec2f res = { 4.0f, 20.0f };
	render->offset = Vec2f{ (animSprite_.getObjRes().abs().x - res.x) / 2, (animSprite_.getObjRes().abs().y - res.y) };

	animSprite_.setAnimation(idle);
	animSprite = animSprite_;
	defaultFrameDelay = animSprite_.frameDelay;

	attackAnimations = weapons.cloneAnimations();
}

void PlayerGC::loadNameTag() {
	EntitySystem::GenEntities(1, &nameTagId);
	EntitySystem::MakeComps<RenderComponent>(1, &nameTagId);
	RenderComponent* nameTagRender = EntitySystem::GetComp<RenderComponent>(nameTagId);
	NDC* nameTagData = EntitySystem::GetComp<NDC>(nameTagId);
	nameTagData->set<float>(X, 0.0f);
	nameTagData->set<float>(Y, 0.0f);

	nameTagRender->loadDrawable<TextDrawable>();


	NDC* data = EntitySystem::GetComp<NDC>(id);
	auto* drawable = nameTagRender->getDrawable<TextDrawable>();
	drawable->color = { 1.0, 1.0, 1.0, 1.0 };
	drawable->anti_alias = true;
	drawable->font.loadDataFile("suqua/fonts/consolas.fnt");
	drawable->scale = { .5, .5 };
	drawable->text = data->get<std::string>(USER_TAG);
	drawable->depth = -0.2;
	nameTagRender->offset = {(drawable->getBoundingBox().res.x / 2), drawable->getBoundingBox().res.y + 25};
}

/*
void PlayerGC::spawnHead(Vec2f pos) {
	unsigned int id_;
	EntitySystem::GenEntities(1, &id_);
	EntitySystem::MakeComps<HeadParticleLC>(1, &id_);
	EntitySystem::MakeComps<RenderComponent>(1, &id_);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id_);

	HeadParticleLC * particle = EntitySystem::GetComp<HeadParticleLC>(id_);
	particle->setLife(240);

	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id_);
	physics->setRes({4, 4});
	physics->weight = 3;
	physics->vel = { 0, -80 };

	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id_);
	position->pos = pos;

	RenderComponent * image = EntitySystem::GetComp<RenderComponent>(id_);
	image->loadDrawable<Sprite>("character");
	image->getDrawable<Sprite>()->setImgOffset({30, 44});
	image->getDrawable<Sprite>()->setObjRes({ 4, 4 });
}
*/

void PlayerGC::updateState(double timeDelta) {
	using State = PlayerLC::State;

	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	NDC* data = EntitySystem::GetComp<NDC>(id);

	State plrState = static_cast<PlayerLC::State>(data->get<char>(STATE));
	Vec2f pos = { data->get<float>(X), data->get<float>(Y) };

	if (!animationFrozen) {
		if (plrState == State::attacking) {
			if (prevState != State::attacking) {
				if (data->get<std::string>(WEAPON_TAG) != currAttackTag) {
					currAttackTag = data->get<std::string>(WEAPON_TAG);
				}
				render->setDrawable<AnimatedSprite>(attackAnimations[currAttackTag]);
			}
			AnimatedSprite& sprite = *render->getDrawable<AnimatedSprite>();
			int width = sprite.getObjRes().abs().x;
			int height = sprite.getObjRes().abs().y;
			sprite.setObjRes(Vec2i{ data->get<int32_t>(DIR) * width, height });

			sprite.frameDelay = defaultFrameDelay / data->get<double>(ATTACK_SPEED);

			sprite.looping = false;

			if (data->get<uint32_t>(ACTIVE_ATTACK) != prevAttack) {
				sprite.setAnimation(data->get<uint32_t>(ACTIVE_ATTACK));
			}
			sprite.forward(timeDelta);
		}
		else {
			if (prevState == State::attacking) {
				render->setDrawable<AnimatedSprite>(animSprite);
			}
			AnimatedSprite& sprite = *render->getDrawable<AnimatedSprite>();
			int width = sprite.getObjRes().abs().x;
			int height = sprite.getObjRes().abs().y;
			sprite.setObjRes(Vec2i{ data->get<int32_t>(DIR) * width, height });

			if (prevState != plrState) {

				sprite.looping = false;
				sprite.frameDelay = defaultFrameDelay;

				switch (plrState) {
				case State::dead:
					sprite.setAnimation(dead);
					shouldSpawnHead = true;
					break;
				case State::rolling:
					sprite.setAnimation(roll);
					break;
				case State::stunned:
					sprite.setAnimation(stun);
					break;
				case State::climbing:
					sprite.setAnimation(climb);
					break;
				case State::crouching:
					sprite.setAnimation(crouch);
					break;
				}
			}

			if (plrState == State::climbing) {
				sprite.frameDelay = defaultFrameDelay / data->get<double>(MOVE_SPEED) * 3;
				sprite.looping = true;
			}

			if (plrState == State::free) {
				sprite.frameDelay = defaultFrameDelay / data->get<double>(MOVE_SPEED);
				sprite.looping = true;
				if (data->get<float>(XVEL) == 0)
					sprite.setAnimation(idle);
				else if (prevState != State::free || prevXVel == 0)
					sprite.setAnimation(walking);

			}

			if (plrState != State::climbing || data->get<float>(XVEL) != 0 || data->get<float>(YVEL) != 0)
				sprite.forward(timeDelta);
		}

		//put this at the end so we don't modify the RenderComponent pool and screw up the sprite reference

		if (shouldSpawnHead && plrState == State::dead) {
			shouldSpawnHead = false;
			Vec2f spawnPos = pos;
			spawnPos.y -= 15;
			Particle p1{ Color{1, 1, 1, 1}, spawnPos, -90, 1.5f, 100, 0 };
			GLRenderer::SpawnParticles("blood", 50, p1, 180.0f, 1.0f, 0.0f, { 2.0f, 2.0f });
		}

		prevXVel = data->get<float>(XVEL);
		prevAttack = data->get<uint32_t>(ACTIVE_ATTACK);
		prevState = plrState;
	}

	if (plrState == State::stunned) {
		if (!wasStunned || data->get<uint32_t>(STUN_FRAME) > prevStunFrame) {
			PhysicsComponent* otherPlayer = EntitySystem::GetComp<PhysicsComponent>(combat->getLastAttacker());
			CombatComponent* otherCombat = EntitySystem::GetComp<CombatComponent>(combat->getLastAttacker());
			if (otherPlayer) {
				auto spawnPos = pos + Vec2f{ 0, -10 };
				float dir = otherPlayer->getPos().x < pos.x;
				dir = dir * 2 - 1;

				Particle p1{ Color{1, 1, 1, 1}, spawnPos, -90 + (45 * dir), 1.8f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 20, p1, 15.0f, 1.0f, 0.0f, { 2.0f, 2.0f });
			}
		}
	}

	prevStunFrame = data->get<uint32_t>(STUN_FRAME);
	wasStunned = plrState == State::stunned;


	//after updating the frame, update wether or not to freeze on that frame
	animationFrozen = combat->isFrozen();

	NDC* nameTagData = EntitySystem::GetComp<NDC>(nameTagId);
	nameTagData->get<float>(X) = pos.x;
	nameTagData->get<float>(Y) = pos.y;
	
	RenderComponent* nameTagRender = EntitySystem::GetComp<RenderComponent>(nameTagId);
	auto* drawable = nameTagRender->getDrawable<TextDrawable>();
	drawable->text = data->get<std::string>(USER_TAG);
	nameTagRender->offset = { (drawable->getBoundingBox().res.x / 2), drawable->getBoundingBox().res.y + 25 };

	EntityBaseComponent* base = EntitySystem::GetComp<EntityBaseComponent>(id);
	EntityBaseComponent* nameTagRenderBase = EntitySystem::GetComp<EntityBaseComponent>(nameTagId);
	nameTagRenderBase->isDead = base->isDead;

}

EntityId PlayerGC::getId() const {
	return id;
}

EntityId PlayerGC::getNameTageRenderId() const {
	return nameTagId;
}
