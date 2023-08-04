#pragma once
#include "EntitySystem.h"
#include "ComponentMacros.h"
#include "Vec2.h"
#include "Color.h"

class CharacterGFXComponent {
	CompMembers(CharacterGFXComponent);
public:
	void loadSpriteSheet(const std::string& tag, const std::string& fileName, Vec2f offset);
	void update(int timeDelta);
	void playAnimation(const std::string& tag, bool looping);
	void stopAnimation();
	void setHasUpDown(bool upDown);
	void setColorOverlay(Color c);
	void setOverlayAmount(float a);
private:
	// run/start the actual animations based on direction, has up down
	void doDefaultAnimations();

	void playDefaultAnimations();
	void startDefaultAnimations();
	bool isPlayingAnimation_;
	int prevCardinalDir;
	Vec2f prevCharacterVel;

	// wether or not this can face up and down, or just left and right
	bool hasUpDown;
};