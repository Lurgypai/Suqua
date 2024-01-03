#include "Level.h"

#include <fstream>
#include <iostream>
#include <exception>

#include "nlohmann/json.hpp"

#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"

using namespace nlohmann;

Level::Level(const std::string& textureTag_, const std::string& fileName_) :
	textureTag{textureTag_},
	fileName{fileName_},
	tiles{},
    grid{},
    levelRes{},
    tileSize{}
{}

Level::Level() :
	textureTag{},
	fileName{},
	tiles{},
    grid{},
    levelRes{},
    tileSize{}
{}

void Level::load(Scene& scene) {
	std::ifstream file{ fileName };
	if (!file.good()) {
		std::cout << "Unable to load level file: " << fileName << '\n';
		throw std::exception{};
	}

	json worldJson{};
    file >> worldJson;
	//worldJson << file;

	if (!worldJson.contains("levels")) {
		std::cout << "ldtk is missing \"levels\"!\n";
		throw std::exception{};
	}

	// we're just gonna load the first level for now.
	const json& topLayer = worldJson["levels"][0]["layerInstances"][0];
	int gridSize = topLayer["__gridSize"];
	Vec2i res{ gridSize, gridSize };
    tileSize = gridSize;
	// std::cout << "res: " << res << '\n';
    
    grid = topLayer["intGridCsv"].get<std::vector<int>>();
    levelRes.x = topLayer["__cWid"];
    levelRes.y = topLayer["__cHei"];

	for (auto& tileJson : topLayer["autoLayerTiles"]) {
		Vec2f worldPos{ tileJson["px"][0], tileJson["px"][1] };
		Vec2f texOffset{ tileJson["src"][0], tileJson["src"][1] };
		EntityId tile = scene.addEntities(1)[0];
		EntitySystem::MakeComps<PhysicsComponent>(1, &tile);
		EntitySystem::MakeComps<RenderComponent>(1, &tile);
		
		auto posComp = EntitySystem::GetComp<PositionComponent>(tile);
		auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(tile);
		auto renderComp = EntitySystem::GetComp<RenderComponent>(tile);

		posComp->setPos(worldPos);
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

void Level::load(const std::string& textureTag_, const std::string& fileName_, Scene& scene) {
    textureTag = textureTag_;
    fileName  = fileName_;

    load(scene);
}

void Level::unload(Scene& scene) {
	for (auto& tile : tiles) {
		auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(tile);
		baseComp->isDead = true;
	}
	tiles.clear();
}

bool Level::hasTile(Vec2f pos) const {
    Vec2i tilePos = Vec2i{static_cast<int>(pos.x / tileSize), static_cast<int>(pos.y / tileSize)};
    int index = tilePos.y * levelRes.x + tilePos.x;
    return grid[index];
}
