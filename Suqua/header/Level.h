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
        Vec2f res;
    };

	Level(const std::string& levelId, const nlohmann::json& levelJson, Scene& scene, const std::string& textureTag);

    Level(const Level& other) = default;
    Level(Level&& other) = default;

    const std::vector<LevelEntity>& getEntities() const;
    const AABB& getBoundingBox() const;
    bool hasTile(const Vec2f& pos) const;

    void activate();
    void deactivate();

    bool isActive() const;
    const std::string& getLevelId() const;
private:
    std::string levelId;

	std::vector<EntityId> tiles;
    std::vector<LevelEntity> entities;

	AABB boundingBox;
	std::vector<int> grid;
	int tileSize;
    bool isActive_;
};
