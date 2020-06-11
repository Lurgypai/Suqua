#include <fstream>
#include <iostream>
#include "WeaponManager.h"

#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

void WeaponManager::loadAttacks(const std::string& attackDir) {
	attacks.loadAttacks(attackDir);
}

void WeaponManager::loadAnimations(std::string animationDir) {

	if (animationDir.back() != '/')
		animationDir += '/';

	for (const auto & pair : attacks) {
		const std::string& id = pair.first;
		json animDesc{};
		const std::string path = animationDir + id + "_anim.json";
		std::cout << "Opening file \"" << path << "\"\n";
		std::ifstream jsonFile{path};
		if (jsonFile.is_open()) {
			try {
				jsonFile >> animDesc;

				Vec2f objRes{ animDesc["frame_width"], animDesc["frame_height"] };
				int columns{ animDesc["columns"] };

				animations.emplace(id, AnimatedSprite{ "weapon::" + id, objRes, columns });
				AnimatedSprite& animation = animations[id];

				//first attack is attack 1
				int i = 1;
				for (auto& hit : animDesc["hits"]) {
					animation.addAnimation(i++, hit["begin_frame"], hit["end_frame"]);
				}
			}
			catch (std::exception e) {
				std::cout << "Error while reading file \"" << path << "\"\n";
				std::cout << e.what() << '\n';
			}
		}
		else {
			std::cout << "Unable to load descriptor for \"" << id << "\".\n";
		}
	}
}

std::unordered_map<std::string, AnimatedSprite> WeaponManager::cloneAnimations() const {
	return animations;
}

bool WeaponManager::hasWeapon(const std::string& attackId) {
	return attacks.hasAttack(attackId);
}

std::vector<std::string> WeaponManager::getAttackTags() {
	return attacks.getAttackTags();
}
