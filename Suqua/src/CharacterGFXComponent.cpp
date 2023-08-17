#include "CharacterGFXComponent.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"
#include "CharacterGFXComponent.h"
#include "DirectionComponent.h"
#include "DebugIO.h"

CharacterGFXComponent::CharacterGFXComponent(EntityId id_) :
	id{ id_ },
	prevCardinalDir{ 0 },
	isPlayingAnimation_{false},
	hasUpDown{false}
{
	if (id != 0)
	{
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
		}
	}
}

void CharacterGFXComponent::loadSpriteSheet(const std::string& tag, const std::string& fileName, Vec2f offset) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	renderComp->loadDrawable<AnimatedSprite>(tag, fileName);
	renderComp->offset = offset;

	auto sprite = renderComp->getDrawable<AnimatedSprite>();
	sprite->looping = true;
}

void CharacterGFXComponent::update(int timeDelta) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();

	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);

	float currCharacterDir = dirComp->getDir();
	int currCardinalDir = 3;
	int currCharacterDirDeg = currCharacterDir * 180 / 3.1415926535898 - 45;
	if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
	if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 90) currCardinalDir = 0;
	else if (currCharacterDirDeg > 90 && currCharacterDirDeg < 180) currCardinalDir = 1;
	else if (currCharacterDirDeg >= 180 && currCharacterDirDeg <= 270) currCardinalDir = 2;


	sprite->update(timeDelta);
	if (isPlayingAnimation_) {
		if (currCardinalDir == 1) {
			sprite->setHorizontalFlip(false);
		}
		else if(currCardinalDir == 3) {
			sprite->setHorizontalFlip(true);
		}
	}
	else {
		playDefaultAnimations();
	}
}

void CharacterGFXComponent::playAnimation(const std::string& tag, bool looping)
{
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();

	if (sprite->hasAnimation(tag)) {
		sprite->setAnimation(tag);
		sprite->looping = looping;
		isPlayingAnimation_ = true;
	}
}

void CharacterGFXComponent::stopAnimation() {
	if (isPlayingAnimation_) {
		isPlayingAnimation_ = false;
		startDefaultAnimations();
	}
}

void CharacterGFXComponent::doDefaultAnimations() {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();
	sprite->looping = true;

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	auto currCharacterVel = physicsComp->getVel();
	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	float currCharacterDir = dirComp->getDir();
	int currCardinalDir = 3;
	if (hasUpDown) {
		int currCharacterDirDeg = currCharacterDir * 180 / 3.1415926535898 - 45;
		if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
		if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 90) currCardinalDir = 0;
		else if (currCharacterDirDeg > 90 && currCharacterDirDeg < 180) currCardinalDir = 1;
		else if (currCharacterDirDeg >= 180 && currCharacterDirDeg <= 270) currCardinalDir = 2;
	}
	else {
		int currCharacterDirDeg = currCharacterDir * 180 / 3.1415926535898 - 90;
		if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
		if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 180) currCardinalDir = 1;
	}

	if (currCharacterVel != Vec2f{0, 0}) {
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
	else {
		sprite->setAnimation("idle");
	}

	prevCardinalDir = currCardinalDir;
	prevCharacterVel = currCharacterVel;
}

void CharacterGFXComponent::playDefaultAnimations()
{
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	auto currCharacterVel = physicsComp->getVel();

	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	float currCharacterDir = dirComp->getDir();
	int currCardinalDir = 3;

	if (hasUpDown) {
		int currCharacterDirDeg = currCharacterDir * 180 / 3.1415926535898 - 45;
		if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
		if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 90) currCardinalDir = 0;
		else if (currCharacterDirDeg > 90 && currCharacterDirDeg < 180) currCardinalDir = 1;
		else if (currCharacterDirDeg >= 180 && currCharacterDirDeg <= 270) currCardinalDir = 2;
	}
	else {
		int currCharacterDirDeg = currCharacterDir * 180 / 3.1415926535898 - 90;
		if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
		if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 180) currCardinalDir = 1;
	}

	//changed facing, update animations
	if (prevCardinalDir != currCardinalDir) {
		doDefaultAnimations();
	}
	//started or stopped moving, update animatisons
	else if (prevCharacterVel != currCharacterVel) {
		if (prevCharacterVel == Vec2f{ 0, 0 } || currCharacterVel == Vec2f{ 0, 0 })
			doDefaultAnimations();
	}
	else {
		prevCardinalDir = currCardinalDir;
		prevCharacterVel = currCharacterVel;
	}
}

void CharacterGFXComponent::startDefaultAnimations() {
	doDefaultAnimations();
}

void CharacterGFXComponent::setHasUpDown(bool upDown) {
	hasUpDown = upDown;
}

void CharacterGFXComponent::setColorOverlay(Color c) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();
	sprite->setColorOverlay(c);
}

void CharacterGFXComponent::setOverlayAmount(float a) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite* sprite = renderComp->getDrawable<AnimatedSprite>();
	sprite->setOverlayAmount(a);
}