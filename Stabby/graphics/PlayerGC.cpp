#include "PlayerGC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "HeadParticleLC.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"
#include "Color.h"
#include "TextDrawable.h"
#include "EntityBaseComponent.h"

#include "../player/OnlinePlayerLC.h"

PlayerGC::PlayerGC(EntityId id_) :
	id{ id_ },
	shouldSpawnHead{ true },
	prevXVel{ 0 },
	prevAttack{ 0 },
	attackAnimations{},
	currAttackTag{},
	animationFrozen{false}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
		}
		if (!EntitySystem::Contains<DirectionComponent>() || EntitySystem::GetComp<DirectionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<DirectionComponent>(1, &id);
			EntitySystem::GetComp<DirectionComponent>(id)->dir = 1;
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
	NameTagComponent* nameTag = EntitySystem::GetComp<NameTagComponent>(id);
	nameTagRender->loadDrawable<TextDrawable>();


	auto* drawable = nameTagRender->getDrawable<TextDrawable>();
	drawable->color = { 1.0, 1.0, 1.0, 1.0 };
	drawable->anti_alias = true;
	drawable->font.loadDataFile("suqua/fonts/consolas.fnt");
	drawable->scale = { .5, .5 };
	drawable->text = nameTag->nameTag;
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
	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
	PlayerState state{};
	if (player) {
		state = player->getState();
	}
	else {
		OnlinePlayerLC* onlinePlayer = EntitySystem::GetComp<OnlinePlayerLC>(id);
		if (onlinePlayer) {
			PlayerStateComponent* stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
			state = stateComp->playerState;
		}
		else {
			throw std::exception{};
		}
	}

	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	DirectionComponent* direction = EntitySystem::GetComp<DirectionComponent>(id);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);

	State plrState = state.state;

	if (!animationFrozen) {
		if (plrState == State::attacking) {
			if (prevState != State::attacking) {
				if (state.weaponTag != currAttackTag) {
					currAttackTag = state.weaponTag;
				}
				render->setDrawable<AnimatedSprite>(attackAnimations[currAttackTag]);
			}
			AnimatedSprite& sprite = *render->getDrawable<AnimatedSprite>();
			int width = sprite.getObjRes().abs().x;
			int height = sprite.getObjRes().abs().y;
			sprite.setObjRes(Vec2i{ direction->dir * width, height });

			sprite.frameDelay = defaultFrameDelay / state.attackSpeed;

			sprite.looping = false;

			if (state.activeAttack != prevAttack) {
				sprite.setAnimation(state.activeAttack);
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
			sprite.setObjRes(Vec2i{ direction->dir * width, height });

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
				sprite.frameDelay = defaultFrameDelay / state.moveSpeed * 3;
				sprite.looping = true;
			}

			if (plrState == State::free) {
				sprite.frameDelay = defaultFrameDelay / state.moveSpeed;
				sprite.looping = true;
				if (state.vel.x == 0)
					sprite.setAnimation(idle);
				else if (prevState != State::free || prevXVel == 0)
					sprite.setAnimation(walking);

			}

			if (plrState != State::climbing || state.vel.x != 0 || state.vel.y != 0)
				sprite.forward(timeDelta);
		}

		prevXVel = state.vel.x;
		prevAttack = state.activeAttack;
		prevState = plrState;

		//put this at the end so we don't modify the RenderComponent pool and screw up the sprite reference

		if (shouldSpawnHead && plrState == State::dead) {
			shouldSpawnHead = false;
			Vec2f spawnPos = state.pos;
			spawnPos.y -= 15;
			Particle p1{ Color{1, 1, 1, 1}, spawnPos, -90, 1.5f, 100, 0 };
			GLRenderer::SpawnParticles("blood", 50, p1, 180.0f, 1.0f, 0.0f, { 2.0f, 2.0f });
		}
	}

	//after updating the frame, update wether or not to freeze on that frame
	animationFrozen = combat->isFrozen();

	PositionComponent* nameTagPos = EntitySystem::GetComp<PositionComponent>(nameTagId);
	nameTagPos->pos = state.pos;
	RenderComponent* nameTagRender = EntitySystem::GetComp<RenderComponent>(nameTagId);
	auto* drawable = nameTagRender->getDrawable<TextDrawable>();
	drawable->text = state.userTag;
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
