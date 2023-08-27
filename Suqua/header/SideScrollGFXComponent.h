#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"
#include <string>

class SideScrollGFXComponent {
	CompMembers(SideScrollGFXComponent);
public:
	void loadSpriteSheet(const std::string& tag, const std::string& fileName, Vec2f offset);
	void update(int timeDelta);

	void playAnimation(const std::string& tag, bool looping);
	void stopAnimation();
	
private:
	void playDefaultAnimations();
	void startDefaultAnimations();
	bool isPlayingAnimation_;

	int prevCardinalDir;
	Vec2f prevCharacterVel;
};