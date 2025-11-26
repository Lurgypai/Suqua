#include "CharacterGFXComponent.h"
#include "RenderComponent.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"
#include "CharacterGFXComponent.h"
#include "DirectionComponent.h"

CharacterGFXComponent::CharacterGFXComponent(
        EntityId id_,
        const std::string& tag,
        const std::string& fileName,
        Vec2f offset,
        Vec2f shadowOffset
        ) :
    sprIndex{},
	id{ id_ },
	prevCardinalDir{ 0 },
	isPlayingAnimation_{false},
	hasUpDown{false}
{
    if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
        EntitySystem::MakeComps<RenderComponent>(1, &id);
    }

    loadSpriteSheet(tag, fileName, offset, shadowOffset);
}

void CharacterGFXComponent::loadSpriteSheet(const std::string& tag, const std::string& fileName,
        Vec2f offset, Vec2f shadowOffset) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	sprIndex = renderComp->loadDrawable<AnimatedSprite>(tag, fileName);

	AnimatedSprite& sprite = renderComp->getDrawable<AnimatedSprite>(sprIndex);
	sprite.looping = true;
    sprite.offset = offset;

    auto index = renderComp->loadDrawable<Sprite>("shadow");
    Sprite& shadowSpr = renderComp->getDrawable<Sprite>(index);
    shadowSpr.offset = shadowOffset;
    shadowSpr.setDepth(.5f);
}

void CharacterGFXComponent::update(int timeDelta) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite& sprite = renderComp->getDrawable<AnimatedSprite>(sprIndex);

	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);

	float currCharacterDir = dirComp->getDir();
	int currCardinalDir = 3;
	int currCharacterDirDeg = currCharacterDir * 180 / 3.1415926535898 - 45;
	if (currCharacterDirDeg < 0) currCharacterDirDeg += 360;
	if (currCharacterDirDeg >= 0 && currCharacterDirDeg <= 90) currCardinalDir = 0;
	else if (currCharacterDirDeg > 90 && currCharacterDirDeg < 180) currCardinalDir = 1;
	else if (currCharacterDirDeg >= 180 && currCharacterDirDeg <= 270) currCardinalDir = 2;


	sprite.update(timeDelta);
	if (isPlayingAnimation_) {
		if (currCardinalDir == 1) {
			sprite.setHorizontalFlip(false);
		}
		else if(currCardinalDir == 3) {
			sprite.setHorizontalFlip(true);
		}
	}
	else {
		playDefaultAnimations();
	}
}

void CharacterGFXComponent::playAnimation(const std::string& tag, bool looping)
{
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite& sprite = renderComp->getDrawable<AnimatedSprite>(sprIndex);

	if (sprite.hasAnimation(tag)) {
		sprite.setAnimation(tag);
		sprite.looping = looping;
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
	AnimatedSprite& sprite = renderComp->getDrawable<AnimatedSprite>(sprIndex);
	sprite.looping = true;

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	const auto& currCharacterVel = physicsComp->vel;
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
		if (currCardinalDir == 0) sprite.setAnimation("down");
		else if (currCardinalDir == 1) {
			sprite.setAnimation("left_right");
			sprite.setHorizontalFlip(false);
		}
		else if (currCardinalDir == 2) sprite.setAnimation("up");
		else {
			sprite.setAnimation("left_right");
			sprite.setHorizontalFlip(true);
		}
	}
	else {
		sprite.setAnimation("idle");
	}

	prevCardinalDir = currCardinalDir;
	prevCharacterVel = currCharacterVel;
}

void CharacterGFXComponent::playDefaultAnimations()
{
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	const auto& currCharacterVel = physicsComp->vel;

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
	renderComp->getDrawable<AnimatedSprite>(sprIndex).setColorOverlay(c);
}

void CharacterGFXComponent::setOverlayAmount(float a) {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
    renderComp->getDrawable<AnimatedSprite>(sprIndex).setOverlayAmount(a);
}
