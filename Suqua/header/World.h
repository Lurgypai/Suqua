#pragma once
#include <string>
#include <unordered_map>

#include "Scene.h"
#include "Level.h"

/*
Levels are imported from ldtk projects. super simple for now, specify a texture to use, and load all of the tiles in as entities
Important, needs to be changed to handle like multiple levels and a world. Change this to a "world" and all of the levels separately or something
for now just loading the whole thing
*/

class World {
public:
    World(const std::string& fileName, Scene& scene, PhysicsSystem& physics);
	
	const std::unordered_map<std::string, Level>& getLevels() const;
	Level* getActiveLevel(const Vec2f& pos, const PhysicsSystem& physics);
	const Level* getActiveLevel(const Vec2f& pos, const PhysicsSystem& physics) const;
    Level& getLevel(const std::string& levelId);
    const Level& getLevel(const std::string& levelId) const;
    bool hasTile(Vec2f pos) const;
private:
    std::unordered_map<std::string, Level> levels;
};

// keep track of neighboring levels on load, store in level
// add default spawn for now, with additional spawns for each room that get triggered on overlap
// for now don't worry about level search times
// 
