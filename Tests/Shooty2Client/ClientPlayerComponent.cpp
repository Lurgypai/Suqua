#include "ClientPlayerComponent.h"
#include "../Shooty2Core/PlayerComponent.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"
#include "ClientPlayerComponent.h"

ClientPlayerComponent::ClientPlayerComponent(EntityId id_) :
	id{ id_ },
	prevPlayerState{ PlayerComponent::State::idle },
	prevPlayerDir{ 0.0f },
	prevCardinalDir{ 0 }
{
	if (id != 0)
	{
		if (!EntitySystem::Contains<PlayerComponent>() || !EntitySystem::GetComp<PlayerComponent>(id)) {
			EntitySystem::MakeComps<PlayerComponent>(1, &id);
		}
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
		}

		auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
		renderComp->loadDrawable<AnimatedSprite>("hero", "stranded/Hero/Hero/Hero.json");
		renderComp->offset = { -13, -24 };

		renderComp->getDrawable<AnimatedSprite>()->looping = true;
	}
}

EntityId ClientPlayerComponent::getId() const {
	return id;
}

void ClientPlayerComponent::updateGFX() {
	auto playerComponent = EntitySystem::GetComp<PlayerComponent>(id);
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();
	auto currPlayerState = playerComponent->getPlayerState();

	float currPlayerDir = playerComponent->getPlayerDir();
	int currCardinalDir = 3;
	int currPlayerDirDeg = currPlayerDir * 180 / 3.1415926535898 - 45;
	if (currPlayerDirDeg < 0) currPlayerDirDeg += 360;
	if (currPlayerDirDeg >= 0 && currPlayerDirDeg <= 90) currCardinalDir = 0;
	else if (currPlayerDirDeg > 90 && currPlayerDirDeg < 180) currCardinalDir = 1;
	else if (currPlayerDirDeg >= 180 && currPlayerDirDeg <= 270) currCardinalDir = 2;


	if (currPlayerState != prevPlayerState) {
		// state change, swap animations

		switch (currPlayerState) {
		case PlayerComponent::State::idle:
			sprite->setAnimation("idle");
			break;
		case PlayerComponent::State::walking:
			if (currCardinalDir == 0) sprite->setAnimation("down");
			else if (currCardinalDir == 1) {
				sprite->setAnimation("left_right");
				sprite->setHorizontalFlip(false);
			}
			else if (currCardinalDir == 2) sprite->setAnimation("up");
			else {
				sprite->setAnimation("left_right");
				sprite->setHorizontalFlip(true);
			}
			break;
		}
	}

	if (currPlayerState == PlayerComponent::State::walking) {
		if (currCardinalDir != prevCardinalDir) {
			if (currCardinalDir == 0) sprite->setAnimation("down");
			else if (currCardinalDir == 1) {
				sprite->setAnimation("left_right");
				sprite->setHorizontalFlip(false);
			}
			else if (currCardinalDir == 2) sprite->setAnimation("up");
			else {
				sprite->setAnimation("left_right");
				sprite->setHorizontalFlip(true);
			}
		}
	}

	prevPlayerDir = currPlayerDir;
	prevPlayerState = currPlayerState;
	prevCardinalDir = currCardinalDir;
}
