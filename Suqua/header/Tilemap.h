#pragma once
#include "Vec2.h"
#include "AABB.h"
#include <vector>

using TilemapGrid = std::vector<std::vector<int>>;

class Tilemap {
public:
    Tilemap(const TilemapGrid& intGrid_, const AABBi& boundingBox_);

    const TilemapGrid& getGrid() const;
    const AABBi& getBoundingBox() const;
    bool hasTile(const Vec2f& pos) const;
    bool contains(const Vec2f& pos) const;
    bool collidesRight(AABB other) const;
private:
    std::vector<std::vector<int>> intGrid;
    AABBi boundingBox;
};
