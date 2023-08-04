#pragma once
#include "Vec2.h"
#include "Sprite.h"
#include "AABB.h"
#include <unordered_map>


//need to override setDepth and other things to use the proper thing
class AnimatedSprite  : public IDrawable {
public:
	AnimatedSprite();
	AnimatedSprite(const std::string& texture_tag, const std::string& json_path);
	void update(int millis);
	int getFrame() const;
	void setFrame(int frame);
	void addAnimation(const std::string& tag, int beginFrame, int endFrame);
	void setAnimation(const std::string& tag);
	void resetDelay();
	Vec2i getAnimation(const std::string& tag) const;
	std::string getCurrentAnimationId() const;
	bool animationIsFinished() const;
	bool hasAnimation(const std::string& tag) const;

	void setColorOverlay(const Color& c);
	void setOverlayAmount(float a);

	void setHorizontalFlip(bool horizFlip);

	IDrawable* clone() override;
	virtual void draw() override;
	virtual void setPos(Vec2f pos) override;

	bool looping;
	float speed;
private:
	struct Frame {
		std::string textureTag;
		AABB obj;
		int duration;
	};
	ImgData data;
	std::vector<Frame> frames;
	std::unordered_map<std::string, Vec2i> animations;
	Vec2i currentAnimation;
	std::string currentAnimationId;
	int currentFrame;
	int currentTime;

	bool horizontalFlip;
	bool verticalFlip;

	bool animationIsFinished_;
};