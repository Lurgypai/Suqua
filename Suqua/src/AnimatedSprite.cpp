#include "AnimatedSprite.h"
#include "DebugIO.h"

AnimatedSprite::AnimatedSprite() : Sprite{"none"} {
}

AnimatedSprite::AnimatedSprite(const std::string &texture_tag_, Vec2f objRes_,
	int columns_, float frameDelay_) :

	Sprite{ texture_tag_ },
	columns{columns_},
	frameDelay{ frameDelay_ },

	currentFrame{ 0 },
	currentTime{ 0 },
	looping{false},
	currentAnimationId{0}
{
	data.objRes = objRes_;
}

void AnimatedSprite::forward(double timeDelta) {
	if (currentFrame < currentAnimation.x)
		currentFrame = currentAnimation.x;

	currentTime += timeDelta;

	int passedFrames = static_cast<int>(currentTime / frameDelay);
	int totalFrames = (currentAnimation.x - currentAnimation.y);
	
	if (passedFrames != 0)
		currentTime -= frameDelay * passedFrames;

	currentFrame += passedFrames;

	if (looping) {
		if (totalFrames != 0)
			currentFrame = ((currentFrame - currentAnimation.x) % totalFrames) + currentAnimation.x;
	}
	else {
		if (currentFrame >= currentAnimation.y)
			currentFrame = currentAnimation.y - 1;
	}

	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

void AnimatedSprite::backward(double timeDelta) {

}

int AnimatedSprite::getFrame() {
	return currentFrame;
}

void AnimatedSprite::setFrame(int frame) {
	currentFrame = frame;
	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

void AnimatedSprite::addAnimation(int id, int beginFrame, int endFrame) {
	animations[id] = { beginFrame, endFrame };
}

void AnimatedSprite::setAnimation(int id) {
	currentAnimationId = id;
	currentAnimation = animations[id];
	currentFrame = currentAnimation.x;
	currentTime = 0.0;
	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

void AnimatedSprite::resetDelay() {
	currentTime = 0.0;
}

Vec2i AnimatedSprite::getAnimation(int id) {
	return animations[id];
}

int AnimatedSprite::getCurrentAnimationId() {
	return currentAnimationId;
}

IDrawable* AnimatedSprite::clone() {
	return new AnimatedSprite(*this);
}