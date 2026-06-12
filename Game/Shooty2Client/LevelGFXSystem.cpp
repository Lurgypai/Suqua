#include "LevelGFXSystem.h"

#include <fstream>
#include <filesystem>

#include "nlohmann/json.hpp"

#include "GLRenderer.h"
#include "Sprite.h"

using namespace nlohmann;

LevelGFXSystem::LevelGFXSystem(const std::string& ldtkFilename) {
	std::ifstream file{ ldtkFilename };
	if (!file.good()) {
        throw std::runtime_error{std::format(
                "World: Unable to open world json file \"{}\"", ldtkFilename
                )};
	}

	json worldJson = json::parse(file);

	if (!worldJson.contains("levels")) {
        throw std::runtime_error{std::format(
                "World: File \"{}\" is missing \"levels\" field", ldtkFilename
                )};
	}

	const json& levelsJson = worldJson["levels"];
	for (auto& levelJson : levelsJson) {
        std::string id = levelJson["identifier"];
        std::string textureTag = "tex:level:" + id;


        for(auto& layer : levelJson["layerInstances"]) {
            std::string layerId = layer["__identifier"];
            if(layerId != "Tiles") continue;

            std::string relTexturePath = layer["__tilesetRelPath"];
            std::filesystem::path ldtkPath = ldtkFilename;
            std::string texturePath = std::string{ldtkPath.parent_path()} + "/" + relTexturePath;
            GLRenderer::LoadTexture(texturePath, textureTag);

            LevelGFX levelGfx{id, textureTag, {}};
            for(auto& tileJson : layer["autoLayerTiles"]) {
                unsigned int f = tileJson["f"];
                levelGfx.tiles.push_back(TileGFX{
                        Vec2f{tileJson["px"][0], tileJson["px"][1]},
                        Vec2i{tileJson["src"][0], tileJson["src"][1]},
                        static_cast<bool>(f & 0b01),
                        static_cast<bool>(f & 0b10)
                        });
            }
            levels.emplace(std::move(id), std::move(levelGfx));
            break;
        }
	}
}

void LevelGFXSystem::draw(const PhysicsSystem& physics, const World& world) {
    Sprite s;
    for(const auto& pair : levels) {
        const Level& level = world.getLevel(pair.first);
        if(!level.isActive()) continue;

        const Tilemap& tilemap = physics.getTilemap(level.getTilemapId());
        for(const auto& tile : pair.second.tiles) {
            s.loadTexture(pair.second.textureTag);
            s.setObjRes(tilemap.getTileRes());

            s.setPos(tile.worldPos);
            s.setImgOffset(tile.texOffset);
            s.setHorizontalFlip(tile.flipHorizontal);
            s.setVerticalFlip(tile.flipVertical);

            s.draw();
        }
    }
}
