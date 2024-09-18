#include "Level.h"
#include "Sprite.h"
#include "EntitySystem.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"

using namespace nlohmann;

Level::Level(const json& levelJson, Scene& scene, const std::string& textureTag) :
	tiles{},
	entities{},
	boundingBox{},
	grid{},
	tileSize{},
    isActive_{true}
{
	Vec2f levelOffset = { levelJson["worldX"], levelJson["worldY"] };
	boundingBox = AABB{ levelOffset, {levelJson["pxWid"], levelJson["pxHei"]} };

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
			tileSize = layerJson["__gridSize"];
			Vec2i res{ tileSize, tileSize };
			grid = layerJson["intGridCsv"].get<std::vector<int>>();

			for (auto& tileJson : layerJson["autoLayerTiles"]) {
				Vec2f worldPos{ tileJson["px"][0], tileJson["px"][1] };
				Vec2f texOffset{ tileJson["src"][0], tileJson["src"][1] };
				EntityId tile = scene.addEntities(1)[0];
				EntitySystem::MakeComps<PhysicsComponent>(1, &tile);
				EntitySystem::MakeComps<RenderComponent>(1, &tile);

				auto posComp = EntitySystem::GetComp<PositionComponent>(tile);
				auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(tile);
				auto renderComp = EntitySystem::GetComp<RenderComponent>(tile);

				posComp->setPos(levelOffset + worldPos);
				physicsComp->setDoesCollide(false);
				physicsComp->setRes(res);

				renderComp->loadDrawable<Sprite>(textureTag);

				auto sprite = renderComp->getDrawable<Sprite>();
				// std::cout << "texture offset: " << texOffset << '\n';
				sprite->setImgOffset(texOffset);
				sprite->setObjRes(res);
				unsigned int f = tileJson["f"];
				// std::cout << "px: " << worldPos << ", f: " << f << '\n';
				sprite->horizontalFlip = f & 1;
				sprite->verticalFlip = f & 2;

				tiles.emplace_back(tile);
			}
		}
	}
}

void Level::activate() {
    for(auto& tile : tiles) {
        auto* baseComp = EntitySystem::GetComp<EntityBaseComponent>(tile);
        baseComp->isActive = true;
    } 
    isActive_ = true;
}

void Level::deactivate() {
    for(auto& tile : tiles) {
        auto* baseComp = EntitySystem::GetComp<EntityBaseComponent>(tile);
        baseComp->isActive = false;
    }
    isActive_ = false;
}

const AABB& Level::getBoundingBox() const {
	return boundingBox;
}

const std::vector<Level::LevelEntity>& Level::getEntities() const {
	return entities;
}

bool Level::hasTile(const Vec2f& pos) const {
	auto inBoundsPos = pos - boundingBox.pos;
    Vec2i tilePos = Vec2i{static_cast<int>(inBoundsPos.x / tileSize), static_cast<int>(inBoundsPos.y / tileSize)};
    int index = tilePos.y * (boundingBox.res.x / tileSize) + tilePos.x;
    return grid[index];
}

bool Level::isActive() const {
    return isActive_;
}

