#pragma once
#include <string>
#include <vector>

#include "Scene.h"
#include "EntitySystem.h"
#include "Level.h"

/*
Levels are imported from ldtk projects. super simple for now, specify a texture to use, and load all of the tiles in as entities
Important, needs to be changed to handle like multiple levels and a world. Change this to a "world" and all of the levels separately or something
for now just loading the whole thing
*/

class World {
public:
    World();
	World(const std::string& textureTag_, const std::string& fileName_);
	void load(Scene& scene);
	void load(const std::string& textureTag_, const std::string& fileName_, Scene& scene);
	
	const std::vector<Level>& getLevels() const;
	const Level* getActiveLevel(const Vec2f& pos) const;
    bool hasTile(Vec2f pos) const;
private:
	std::string textureTag;
	std::string fileName;
	
	std::vector<Level> levels;
};

// keep track of neighboring levels on load, store in level
// add default spawn for now, with additional spawns for each room that get triggered on overlap
// for now don't worry about level search times
// 
