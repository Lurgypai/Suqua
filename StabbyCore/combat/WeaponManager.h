#pragma once
#include "AttackManager.h"
#include "AnimatedSprite.h"

class WeaponManager {
public:
	//load attacks from the specified folder
	void loadAttacks(const std::string& attackDir);
	//load animations for the attacks, from the specified folder (requires loaded attacks)
	void loadAnimations(std::string animationDir);

	Attack cloneAttack(const std::string& attackId) const;
	AnimatedSprite cloneAnimation(const std::string& attackId);

	bool hasWeapon(const std::string & attackId);
private:
	AttackManager attacks;
	std::unordered_map<std::string, AnimatedSprite> animations;
};