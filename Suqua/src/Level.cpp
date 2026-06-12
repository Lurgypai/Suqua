#include "Level.h"

#include "PhysicsComponent.h"

using namespace nlohmann;
using UUID = Suqua::UUID;

Level::Level(const std::string& levelId_, const json& levelJson, PhysicsSystem& physics, Scene& scene) :
    levelId{ levelId_ },
	entities{},
    isActive_{true}
{
	Vec2f levelOffset = { levelJson["worldX"], levelJson["worldY"] };
	AABBi boundingBox = AABBi{ levelOffset, {levelJson["pxWid"], levelJson["pxHei"]} };

	for (auto& layerJson : levelJson["layerInstances"]) {
		const std::string& identifier = layerJson["__identifier"];
		if (identifier == "Entities") {
			for (auto& entityJson : layerJson["entityInstances"]) {
				std::string identifier = entityJson["__identifier"];

                Vec2f res = {entityJson["width"], entityJson["height"]};
				Vec2f spawnPos = Vec2f{ entityJson["__worldX"], entityJson["__worldY"]} +
					Vec2f{ res.x / 2.f, res.y };

				entities.emplace_back(LevelEntity{ identifier, spawnPos, res });
			}
		}
		else if (identifier == "Tiles") {
			int tileSize = layerJson["__gridSize"];
			Vec2i tileRes{ tileSize, tileSize };
            std::vector<int> grid = layerJson["intGridCsv"].get<std::vector<int>>();

            tilemapId = physics.loadTileMap(Tilemap{
                    tileRes,
                    std::move(grid),
                    boundingBox
                });
		}
	}

    deactivate();
}

void Level::activate() {
    isActive_ = true;
}

void Level::deactivate() {
    isActive_ = false;
}

const std::vector<Level::LevelEntity>& Level::getEntities() const {
	return entities;
}

bool Level::isActive() const {
    return isActive_;
}

const std::string& Level::getLevelId() const {
    return levelId;
}

TilemapId Level::getTilemapId() const {
    return tilemapId;
}
