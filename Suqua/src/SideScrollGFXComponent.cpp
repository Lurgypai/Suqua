#include "SideScrollGFXComponent.h"
#include "RenderComponent.h"
#include "AnimatedSprite.h"
#include "DirectionComponent.h"
#include "PhysicsComponent.h"

SideScrollGFXComponent::SideScrollGFXComponent(EntityId id_) :
	id{ id_ },
	isPlayingAnimation_{false},
	prevCardinalDir{ 0 },
	prevCharacterVel{ 0, 0 }
{
	if (id != 0)
	{
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
		}
	}
}

void SideScrollGFXComponent::loadSpriteSheet(const std::string& tag, const std::string& fileName, Vec2f offset) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	renderComp->loadDrawable<AnimatedSprite>(tag, fileName);
	renderComp->offset = offset;

	auto sprite = renderComp->getDrawable<AnimatedSprite>();
	sprite->looping = true;
}

void SideScrollGFXComponent::update(int timeDelta) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();

	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);

	int currCardinalDir = dirComp->getCardinalDir();

	sprite->update(timeDelta);
	if (isPlayingAnimation_) {
		if (currCardinalDir == 1) {
			sprite->setHorizontalFlip(false);
		}
		else if (currCardinalDir == 3) {
			sprite->setHorizontalFlip(true);
		}
	}
	else {
		playDefaultAnimations();
	}
}

void SideScrollGFXComponent::startDefaultAnimations() {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();
	sprite->looping = true;

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	auto currCharacterVel = physicsComp->getVel();
	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	int currCardinalDir = dirComp->getCardinalDir();

	std::cout << "STARTING\n";
	if (currCharacterVel != Vec2f{ 0, 0 }) {
		if ((currCharacterVel.x < 0 && currCardinalDir == 1) || (currCharacterVel.x > 0 && currCardinalDir == 3)) {
			sprite->setAnimation("turn");
			std::cout << "TURNING\n";
		}
		else {
			sprite->setAnimation("left_right");
			std::cout << "WALKING\n";
		}

		if (currCardinalDir == 1) {
			sprite->setHorizontalFlip(false);
		}
		else if (currCardinalDir == 3) {
			sprite->setHorizontalFlip(true);
		}
	}
	else {
		sprite->setAnimation("idle");
		std::cout << "IDLE\n";
	}
}

void SideScrollGFXComponent::playDefaultAnimations()
{
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	auto currCharacterVel = physicsComp->getVel();

	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	int currCardinalDir = dirComp->getCardinalDir();


	//changed facing, update animations
	if (prevCardinalDir != currCardinalDir) {
		startDefaultAnimations();
		
	}
	//started or stopped moving, update animatisons
	else if (prevCharacterVel != currCharacterVel) {
		if (prevCharacterVel == Vec2f{ 0, 0 } || currCharacterVel == Vec2f{ 0, 0 })
			startDefaultAnimations();

		//turned
		if ((prevCharacterVel.x <= 0 && currCharacterVel.x > 0) || (prevCharacterVel.x >= 0 && currCharacterVel.x < 0))
			startDefaultAnimations();
	}

	prevCardinalDir = currCardinalDir;
	prevCharacterVel = currCharacterVel;
}

void SideScrollGFXComponent::playAnimation(const std::string& tag, bool looping)
{
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();

	if (sprite->hasAnimation(tag)) {
		sprite->setAnimation(tag);
		sprite->looping = looping;
		isPlayingAnimation_ = true;
	}
}

void SideScrollGFXComponent::stopAnimation() {
	if (isPlayingAnimation_) {
		isPlayingAnimation_ = false;
		startDefaultAnimations();
	}
}