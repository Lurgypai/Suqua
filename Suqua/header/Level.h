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
    struct LevelEntity {
        std::string id;
        Vec2f pos;
    };

    Level();
	Level(const std::string& textureTag_, const std::string& fileName_);
	void load(Scene& scene);
	void load(const std::string& textureTag_, const std::string& fileName_, Scene& scene);
	void unload(Scene& scene);

    bool hasTile(Vec2f pos) const;

    const std::vector<LevelEntity>& getEntities() const;
private:
	std::string textureTag;
	std::string fileName;

	std::vector<EntityId> tiles;
    std::vector<LevelEntity> entities;

    // a tilemap dictating what tiles are solid for easy querying
    std::vector<int> grid;
    Vec2i levelRes;
    int tileSize;

};
