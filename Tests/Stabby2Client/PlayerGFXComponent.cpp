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
		}
	}
}

EntityId PlayerGFXComponent::getId() const {
	return id;
}

void PlayerGFXComponent::update(int millis) {
	auto* render = EntitySystem::GetComp<RenderComponent>(id);
	auto* sprite = render->getDrawable<AnimatedSprite>();
	sprite->update(millis);

	auto* player = EntitySystem::GetComp<PlayerComponent>(id);
	auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (prevState != player->getState()) {
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
		}
	}

	switch (player->getState()) {
	case PlayerComponent::PlayerState::walking:
		if (physics->getVel().x < 0) {
			sprite->setHorizontalFlip(true);
		}
		else {
			sprite->setHorizontalFlip(false);
		}
	}

	prevState = player->getState();
}
