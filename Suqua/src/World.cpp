#include "World.h"

#include <fstream>

#include "nlohmann/json.hpp"

using namespace nlohmann;

World::World(const std::string& fileName, Scene& scene, PhysicsSystem& physics) :
    levels{}
{
	std::ifstream file{ fileName };
	if (!file.good()) {
        throw std::runtime_error{std::format(
                "World: Unable to open world json file \"{}\"", fileName
                )};
	}

	json worldJson = json::parse(file);

	if (!worldJson.contains("levels")) {
        throw std::runtime_error{std::format(
                "World: File \"{}\" is missing \"levels\" field", fileName
                )};
	}

	const json& levelsJson = worldJson["levels"];
	for (auto& levelJson : levelsJson) {
        std::string id = levelJson["identifier"];
		levels.emplace(id, Level{id, levelJson, physics, scene});
	}
}


Level* World::getActiveLevel(const Vec2f& pos, const PhysicsSystem& physics) {
	for (auto& pair : levels) {
        auto& level = pair.second;
		if (physics.getTilemap(level.getTilemapId()).contains(pos)) return &level;
	}
	return nullptr;
}

const Level* World::getActiveLevel(const Vec2f& pos, const PhysicsSystem& physics) const {
	for (auto& pair : levels) {
        auto& level = pair.second;
		if (physics.getTilemap(level.getTilemapId()).contains(pos)) return &level;
	}
	return nullptr;
}

const std::unordered_map<std::string, Level>& World::getLevels() const {
	return levels;
}

Level& World::getLevel(const std::string& levelId) {
    return levels.at(levelId);
}

const Level& World::getLevel(const std::string& levelId) const {
    return levels.at(levelId);
}
