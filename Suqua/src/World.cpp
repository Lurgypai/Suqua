#include "World.h"

#include <fstream>
#include <iostream>
#include <exception>

#include "nlohmann/json.hpp"

using namespace nlohmann;

World::World(const std::string& textureTag_, const std::string& fileName_) :
	textureTag{textureTag_},
	fileName{fileName_}
{}

World::World() :
	textureTag{},
	fileName{}
{}

void World::load(Scene& scene) {
	std::ifstream file{ fileName };
	if (!file.good()) {
		std::cout << "Unable to load World file: " << fileName << '\n';
		throw std::exception{};
	}

	json worldJson{};
    file >> worldJson;
	//worldJson << file;

	if (!worldJson.contains("levels")) {
		std::cout << "ldtk is missing \"levels\"!\n";
		throw std::exception{};
	}

	const json& levelsJson = worldJson["levels"];
	for (auto& levelJson : levelsJson) {
		levels.emplace_back(levelJson, scene, textureTag);
	}
}

void World::load(const std::string& textureTag_, const std::string& fileName_, Scene& scene) {
    textureTag = textureTag_;
    fileName  = fileName_;

    load(scene);
}

const Level* World::getActiveLevel(const Vec2f& pos) const {
	for (auto& level : levels) {
		if (level.getBoundingBox().contains(pos)) return &level;
	}
	return nullptr;
}

bool World::hasTile(Vec2f pos) const {
	const Level* level = getActiveLevel(pos);
	if (level == nullptr) return false;
	return level->hasTile(pos);
}

const std::vector<Level>& World::getLevels() const {
	return levels;
}

std::vector<Level>& World::getLevels() {
    return levels;
}
