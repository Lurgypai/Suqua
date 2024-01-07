#pragma once
#include "nlohmann/json.hpp"

#include "Scene.h"
#include "AABB.h"
#include "Vec2.h"

class Level {
public:
    struct LevelEntity {
        std::string id;
        Vec2f pos;
    };

	Level(const nlohmann::json& levelJson, Scene& scene, const std::string& textureTag);

    const std::vector<LevelEntity>& getEntities() const;
    const AABB& getBoundingBox() const;
    bool hasTile(const Vec2f& pos) const;
private:
	std::vector<EntityId> tiles;
    std::vector<LevelEntity> entities;

	AABB boundingBox;
	std::vector<int> grid;
	int tileSize;
};