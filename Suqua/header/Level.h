#pragma once
#include <string>
#include <vector>

#include "Scene.h"
#include "EntitySystem.h"

/*
Levels are imported from ldtk projects. super simple for now, specify a texture to use, and load all of the tiles in as entities
Important, needs to be changed to handle like multiple levels and a world. Change this to a "world" and all of the levels separately or something
for now just loading the whole thing
*/

class Level {
public:
	Level(const std::string& textureTag_, const std::string& fileName_);
	void load(Scene& scene);
	void unload(Scene& scene);

private:
	std::string textureTag;
	std::string fileName;

	std::vector<EntityId> tiles;
};