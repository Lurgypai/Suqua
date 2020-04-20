#pragma once
#include "Vec2.h"
#include "Sprite.h"
#include <unordered_map>

class AnimatedSprite  : public Sprite {
public:
	AnimatedSprite();
	AnimatedSprite(const std::string &filePath, Vec2f objRes_,
		int columns_ = 8, float frameDelay = (4.0 / 60),
		Vec2f pos_ = {0.0, 0.0}, Vec2f origin_ = { 0.0, 0.0 }, Vec2f scale_ = { 1.0, 1.0 }, Vec2f imgOffset_ = { 0.0f, 0.0f });
	void forward(double timeDelta);
	void backward(double timeDelta);
	int getFrame();
	void setFrame(int frame);
	void addAnimation(int id, int beginFrame, int endFrame);
	void setAnimation(int id);
	void resetDelay();
	Vec2i getAnimation(int id);
	int getCurrentAnimationId();

	IDrawable* clone() override;

	bool looping;
	double frameDelay;
private:
	std::unordered_map<int, Vec2i> animations;
	Vec2i currentAnimation;
	int currentAnimationId;
	int columns;
	int sheetOffset;
	int currentFrame;
	double currentTime;
};