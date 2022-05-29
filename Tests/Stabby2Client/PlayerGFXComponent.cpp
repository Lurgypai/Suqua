#include "PlayerGFXComponent.h"
#include "RenderComponent.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"
#include "../Stabby2Core/PlayerComponent.h"

PlayerGFXComponent::PlayerGFXComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
			auto* render = EntitySystem::GetComp<RenderComponent>(id);
			render->loadDrawable<AnimatedSprite>("stabbyman", "stabbyman.json");
			render->offset = { 31, 28 };
		}
	}
}

EntityId PlayerGFXComponent::getId() const {
	return id;
}

void PlayerGFXComponent::update(int millis) {
	auto* render = EntitySystem::GetComp<RenderComponent>(id);
	auto* sprite = render->getDrawable<AnimatedSprite>();

	auto* player = EntitySystem::GetComp<PlayerComponent>(id);
	auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (player->getState() != PlayerComponent::PlayerState::freeze) sprite->update(millis);

	if (prevState != player->getState() && prevState != PlayerComponent::PlayerState::freeze) {
		switch (player->getState()) {
		case PlayerComponent::PlayerState::idle:
			sprite->setAnimation("Idle");
			break;
		case PlayerComponent::PlayerState::walking:
			sprite->looping = true;
			sprite->setAnimation("Walk");
			break;
		case PlayerComponent::PlayerState::jumpsquat:
			sprite->looping = false;
			sprite->setAnimation("Jumpsquat");
			break;
		case PlayerComponent::PlayerState::airborn:
			sprite->setAnimation("Idle");
			break;
		case PlayerComponent::PlayerState::landing:
			sprite->looping = false;
			sprite->setAnimation("Landing");
			break;
		case PlayerComponent::PlayerState::dodge:
			sprite->looping = false;
			sprite->setAnimation("Roll");
			break;
		case PlayerComponent::PlayerState::airdodge:
			sprite->looping = false;
			sprite->setAnimation("Airdodge");
			break;
		case PlayerComponent::PlayerState::hitstun:
			sprite->looping = false;
			sprite->setAnimation("Hurt");
			break;
		case PlayerComponent::PlayerState::grounded_attack:
			switch (player->getCurrAttackId()) {
			case PlayerComponent::PlayerAttack::slash1:
				sprite->looping = false;
				sprite->setAnimation("Slash1");
				break;
			case PlayerComponent::PlayerAttack::slash2:
				sprite->looping = false;
				sprite->setAnimation("Slash2");
				break;
			case PlayerComponent::PlayerAttack::slash3:
				sprite->looping = false;
				sprite->setAnimation("Slash3");
				break;
			}
			break;
		}
	}

	switch (player->getState()) {
	case PlayerComponent::PlayerState::walking:
	case PlayerComponent::PlayerState::dodge:
		if (physics->getVel().x < 0) {
			sprite->setHorizontalFlip(true);
		}
		else {
			sprite->setHorizontalFlip(false);
		}
	}

	if (prevAttack != player->getCurrAttackId()) {
		switch (player->getCurrAttackId()) {
		case PlayerComponent::PlayerAttack::slash1:
			sprite->looping = false;
			sprite->setAnimation("Slash1");
			break;
		case PlayerComponent::PlayerAttack::slash2:
			sprite->looping = false;
			sprite->setAnimation("Slash2");
			break;
		case PlayerComponent::PlayerAttack::slash3:
			sprite->looping = false;
			sprite->setAnimation("Slash3");
			break;
		}
	}

	if(player->getState() != PlayerComponent::PlayerState::freeze) prevState = player->getState();
	prevAttack = player->getCurrAttackId();
}
