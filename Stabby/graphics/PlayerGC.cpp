
#include "PlayerGC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "HeadParticleLC.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"


PlayerGC::PlayerGC(EntityId id_) :
	id{ id_ },
	shouldSpawnHead{true},
	prevXVel{0},
	prevAttack{0}
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

void PlayerGC::loadAnimations() {
	RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite & animSprite_ = *render->getDrawable<AnimatedSprite>();


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
}

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

void PlayerGC::updateState(double timeDelta) {
	PlayerStateComponent * player = EntitySystem::GetComp<PlayerStateComponent>(id);
	if (player != nullptr) {
		RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
		DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(id);
		PlayerState state = player->playerState;

		State plrState = state.state;

		if (state.attackFreezeFrame == 0) {
			if (plrState == State::attacking) {
				if (prevState != State::attacking) {
					render->setDrawable<AnimatedSprite>(attackSprite);
					
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

				if(plrState != State::climbing || state.vel.x != 0 || state.vel.y != 0)
					sprite.forward(timeDelta);
			}

			prevXVel = state.vel.x;
			prevAttack = state.activeAttack;
			prevState = plrState;

			//put this at the end so we don't modify the RenderComponent pool and screw up the sprite reference
			if (shouldSpawnHead && plrState == State::dead) {
				shouldSpawnHead = false;
				PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
				spawnHead(position->pos);
			}
			/*
			if (plrState == State::stunned && attacker->playerState.activeAttack != prevAttackerAttack) {
				float offset = 7;
				Vec2f spawnPos = attacker->playerState.pos;
				int dir = spawnPos.x < state.pos.x ? 1 : -1;
				spawnPos.x += offset * dir;
				spawnPos.y -= 15;

				Particle p1{ spawnPos, -90 + 36.0f * dir, 1.6f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 5, p1, 5.0f, 0.0f, 0.0f, { 0.3f, 3.0f });
			}

			if(attacker != nullptr)
				prevAttackerAttack = attacker->playerState.activeAttack;
				*/
		}
	}
}

EntityId PlayerGC::getId() const {
	return id;
}
