#include "AnimatedSprite.h"
#include "DebugIO.h"
#include "nlohmann/json.hpp"
#include <fstream>

#include <iostream>

using json = nlohmann::json;

AnimatedSprite::AnimatedSprite(const std::string& texture_tag, const std::string& json_path) :
	currentTime{ 0 },
	speed{1.0f},
	looping{false}
{
	std::ifstream file{ json_path };
	if (!file.good()) {
		//bad juju
		throw std::exception{};
	}
	json data{};
	data << file;
	if (!data.contains("frames")) {
		throw std::exception();
	}
	json frameData = data["frames"];
	frames.reserve(frameData.size());
	for (int i = 0; i != frameData.size(); ++i) {
		json frame = frameData[i];
		frames.emplace_back(Frame{
			Sprite{texture_tag},
			AABB{{frame["frame"]["x"], frame["frame"]["y"]}, {frame["frame"]["w"], frame["frame"]["h"]}},
			frame["duration"]
			});
		Frame& f = frames.back();
		f.s.setObjRes(f.obj.res);
		f.s.setImgOffset(f.obj.pos);
	}
	json animationData = data["meta"]["frameTags"];
	for (int i = 0; i != animationData.size(); ++i) {
		json animation = animationData[i];
		animations.emplace(
			animation["name"],
			Vec2i{
				static_cast<int32_t>(animation["from"]),
				static_cast<int32_t>(animation["to"]) + 1
			}
		);
		//defaults
		if (i == 0) {
			currentAnimationId = animation["name"];
			currentAnimation = animations.at(currentAnimationId);
			currentFrame = currentAnimation.x;
		}
	}
	//load animations
}

void AnimatedSprite::update(int millis) {
	currentTime += millis * speed;
	
	Frame* currFrame = &frames.at(currentFrame);
	while (currentTime > currFrame->duration) {
		currentTime -= currFrame->duration;
		++currentFrame;
		if (currentFrame == currentAnimation.y) {
			if (looping) currentFrame = currentAnimation.x;
			else currentFrame = currentAnimation.y - 1;
		}
		currFrame = &frames.at(currentFrame);
	}
}

int AnimatedSprite::getFrame() const {
	return currentFrame;
}

void AnimatedSprite::setFrame(int frame) {
	currentFrame = frame;
}

void AnimatedSprite::addAnimation(const std::string& tag, int beginFrame, int endFrame) {
	animations[tag] = { beginFrame, endFrame };
}

void AnimatedSprite::setAnimation(const std::string& tag) {
	currentAnimationId = tag;
	currentAnimation = animations.at(tag);
	currentFrame = currentAnimation.x;
	currentTime = 0;
}

void AnimatedSprite::resetDelay() {
	currentTime = 0;
}

Vec2i AnimatedSprite::getAnimation(const std::string& tag) const {
	return animations.at(tag);
}

std::string AnimatedSprite::getCurrentAnimationId() const {
	return currentAnimationId;
}

void AnimatedSprite::setHorizontalFlip(bool horizFlip) {
	for (auto& f : frames) {
		f.s.horizontalFlip = horizFlip;
	}
}

IDrawable* AnimatedSprite::clone() {
	return new AnimatedSprite(*this);
}

void AnimatedSprite::draw() {
	frames.at(currentFrame).s.draw();
}

void AnimatedSprite::setPos(Vec2f pos) {
	for (auto& f : frames) {
		f.s.setPos(pos);
	}
}
