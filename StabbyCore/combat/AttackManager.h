#pragma once
#include <string>
#include <unordered_map>
#include <list>

#include "Attack.h"

using attack_iterator = std::unordered_map<std::string, Attack>::const_iterator;

class AttackManager {
public:
	void loadAttacks(const std::string & attackDir);
	Attack cloneAttack(const std::string & attackId) const;

	attack_iterator begin();
	attack_iterator end();

	bool hasAttack(const std::string& attackId);

private:
	std::unordered_map<std::string, Attack> attacks;
};
