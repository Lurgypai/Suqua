#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "PhysicsSystem.h"
#include "World.h"

struct TileGFX {
    Vec2f worldPos;
    Vec2i texOffset;
    bool flipHorizontal;
    bool flipVertical;
};

struct LevelGFX {
    std::string levelTag;
    std::string textureTag;
    std::vector<TileGFX> tiles;
};

class LevelGFXSystem {
public:
    LevelGFXSystem(const std::string& ldtkFilename);
    // physics is used to grab tilemaps to check where we are so we know what to render
    void draw(const PhysicsSystem& physics, const World& world);
private:
    std::unordered_map<std::string, LevelGFX> levels;
};
