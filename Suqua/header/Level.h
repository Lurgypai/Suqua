#pragma once
#include "nlohmann/json.hpp"

#include "PhysicsSystem.h"
#include "Scene.h"
#include "Vec2.h"

class Level {
public:
    struct LevelEntity {
        std::string id;
        Vec2f pos;
        Vec2f res;
    };

	Level(const std::string& levelId, const nlohmann::json& levelJson,
            PhysicsSystem& physics, Scene& scene);

    Level(const Level& other) = default;
    Level(Level&& other) = default;

    const std::vector<LevelEntity>& getEntities() const;

    void activate();
    void deactivate();

    bool isActive() const;
    const std::string& getLevelId() const;
    TilemapId getTilemapId() const;
private:
    std::string levelId;
    TilemapId tilemapId;

    std::vector<LevelEntity> entities;

    bool isActive_;
};
