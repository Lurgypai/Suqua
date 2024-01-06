#include "World.h"

#include <fstream>
#include <iostream>
#include <exception>

#include "nlohmann/json.hpp"

#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"

using namespace nlohmann;

World::World(const std::string& textureTag_, const std::string& fileName_) :
	textureTag{textureTag_},
	fileName{fileName_},
	tiles{}
{}

World::World() :
	textureTag{},
	fileName{},
	tiles{}
{}

void World::load(Scene& scene) {
	std::ifstream file{ fileName };
	if (!file.good()) {
		std::cout << "Unable to load World file: " << fileName << '\n';
		throw std::exception{};
	}

	json worldJson{};
    file >> worldJson;
	//worldJson << file;

	if (!worldJson.contains("levels")) {
		std::cout << "ldtk is missing \"levels\"!\n";
		throw std::exception{};
	}

	const json& levelsJson = worldJson["levels"];
	for (auto& levelJson : levelsJson) {

		Vec2f levelOffset = { levelJson["worldX"], levelJson["worldY"] };
		camBoxes.emplace_back(CameraBox{ AABB{ levelOffset, {levelJson["pxWid"], levelJson["pxHei"]}} });

		for (auto& layerJson : levelJson["layerInstances"]) {
			const std::string& identifier = layerJson["__identifier"];
			if (identifier == "Entities") {
				for (auto& entityJson : layerJson["entityInstances"]) {
					std::string identifier = entityJson["__identifier"];

					float width = entityJson["width"];
					Vec2f spawnPos = Vec2f{ entityJson["__worldX"], entityJson["__worldY"]} +
						Vec2f{ width / 2.f, entityJson["height"] };

					entities.emplace_back(LevelEntity{ identifier, spawnPos });
				}
			}
			else if (identifier == "Tiles") {
				int gridSize = layerJson["__gridSize"];
				Vec2i res{ gridSize, gridSize };
				camBoxes.back().tileSize = gridSize;
				// std::cout << "res: " << res << '\n';

				camBoxes.back().grid = layerJson["intGridCsv"].get<std::vector<int>>();

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
}

void World::load(const std::string& textureTag_, const std::string& fileName_, Scene& scene) {
    textureTag = textureTag_;
    fileName  = fileName_;

    load(scene);
}

void World::unload(Scene& scene) {
	for (auto& tile : tiles) {
		auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(tile);
		baseComp->isDead = true;
	}
	tiles.clear();
}

bool World::hasTile(Vec2f pos) const {
	auto* box = getCamBox(pos);
	if (box == nullptr) return false;

	auto inBoundsPos = pos - box->box.pos;
    Vec2i tilePos = Vec2i{static_cast<int>(inBoundsPos.x / box->tileSize), static_cast<int>(inBoundsPos.y / box->tileSize)};
    int index = tilePos.y * (box->box.res.x / box->tileSize) + tilePos.x;
    return box->grid[index];
}

const std::vector<World::LevelEntity>& World::getEntities() const {
    return entities;
}

const World::CameraBox* World::getCamBox(const Vec2f& pos) const {
	for (auto& camBox : camBoxes) {
		if (camBox.box.contains(pos)) return &camBox;
	}
	return nullptr;
}