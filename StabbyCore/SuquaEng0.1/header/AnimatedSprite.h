#pragma once
#include "Vec2.h"
#include "Sprite.h"
#include <unordered_map>

class AnimatedSprite  : public Sprite {
public:
	AnimatedSprite(const std::string &filePath, Vec2f objRes_,
		int columns_ = 8, int totalSteps = 4,
		Vec2f pos_ = {0.0, 0.0}, Vec2f origin_ = { 0.0, 0.0 }, Vec2f scale_ = { 1.0, 1.0 }, Vec2f imgOffset_ = { 0.0f, 0.0f });
	void forward();
	void backward();
	int getFrame();
	void setFrame(int frame);
	void addAnimation(int id, int beginFrame, int endFrame);
	void setAnimation(int id);
	void resetDelay();
	Vec2i getAnimation(int id);
private:
	std::unordered_map<int, Vec2i> animations;
	Vec2i currentAnimation;
	int columns;
	int sheetOffset;
	int currentFrame;
	int totalFrames;
	int currentStep;
	int totalSteps;
};