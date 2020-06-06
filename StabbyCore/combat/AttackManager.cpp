#include "AttackTag.h"
#include "AttackManager.h"
#include "CombatComponent.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include "../include/nlohmann/json.hpp"
using json = nlohmann::json;

namespace fs = std::filesystem;

void AttackManager::loadAttacks(const std::string & attackDir) {
	fs::path path{ attackDir };
	for (auto & file : fs::directory_iterator(path)) {
		if (file.path().extension() == ".json") {
			std::string id = file.path().stem().string();
			if (id.size() < WEAPON_TAG_SIZE) {
				std::ifstream stream{ file.path().string() };
				if (stream.is_open()) {
					try {
						json jsonFile;
						stream >> jsonFile;

						int restartDelayMax = jsonFile["restartDelay"];

						Attack attack{ id, restartDelayMax };

						for (auto& hitbox : jsonFile["hitboxes"]) {
							Vec2f res{}, offset{};

							res.x = hitbox["width"];
							res.y = hitbox["height"];
							offset.x = hitbox["offset_x"];
							offset.y = hitbox["offset_y"];

							unsigned int startup = hitbox["startup"];
							unsigned int active = hitbox["active"];
							unsigned int ending = hitbox["ending"];

							auto hitboxStats = hitbox["stats"];
							AttackStats stats{};

							stats.staminaCost = hitboxStats["staminaCost"];

							stats.damage = hitboxStats["damage"];
							stats.stun = hitboxStats["stun"];

							stats.critChance = hitboxStats["critChance"];
							stats.critMultiplier = hitboxStats["critMultiplier"];

							stats.vampirismChance = hitboxStats["vampirismChance"];
							stats.vampirismMultiplier = hitboxStats["vampirismMultiplier"];

							attack.addHitbox(Hitbox{ AABB{{0 ,0}, res}, offset, startup, active, ending, stats });
						}

						attacks.insert(std::pair{ id, attack });
					}
					catch (std::exception e) {
						std::cout << "An error occurred while reading the file \"" << file.path() << "\"\n";
						std::cout << e.what() << '\n';
					}
				}
				else {
					std::cout << "Unable to open file \"" << file.path() << "\"\n";
				}
			}
			else {
				std::cout << "Weapon name " << id << " to large. Must be 10 or less characters.\n";
			}
		}
	}

	CombatComponent::AttackMap = attacks;
}

attack_iterator AttackManager::begin() {
	return attacks.begin();
}

attack_iterator AttackManager::end() {
	return attacks.end();
}

bool AttackManager::hasAttack(const std::string& attackId) {
	return attacks.find(attackId) != attacks.end();
}

std::vector<std::string> AttackManager::getAttackTags() {
	std::vector<std::string> attackTags;
	for (auto& pair : attacks) {
		attackTags.push_back(pair.first);
	}
	return std::move(attackTags);
}
