#pragma once
#include "AttackManager.h"
#include "AnimatedSprite.h"
#include "CharBuffer.h"
#include "AttackTag.h"

class WeaponManager {
public:
	//load attacks from the specified folder
	void loadAttacks(const std::string& attackDir);
	//load animations for the attacks, from the specified folder (requires loaded attacks)
	void loadAnimations(std::string animationDir);

	std::unordered_map<std::string, AnimatedSprite> cloneAnimations() const;

	bool hasWeapon(const std::string & attackId);
	std::vector<std::string> getAttackTags();
private:
	AttackManager attacks;
	std::unordered_map<std::string, AnimatedSprite> animations;
};