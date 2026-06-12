#pragma once
#include "Vec2.h"
#include "AABB.h"
#include <vector>

using TilemapGrid = std::vector<int>;

class Tilemap {
public:
    Tilemap(const Vec2i& tileRes, TilemapGrid&& intGrid_, const AABBi& boundingBox_);

    const TilemapGrid& getGrid() const;
    const AABBi& getBoundingBox() const;
    bool hasTile(const Vec2f& pos) const;
	bool hasTileInMap(const Vec2i& tilePos) const;
    bool contains(const Vec2f& pos) const;
	bool intersects(const AABB& other) const;
    bool collidesRight(AABB other) const;
	const Vec2i& getTileRes() const;
	Vec2i getTilePos(const Vec2f& pos) const;
private:
    Vec2i tileRes;
    TilemapGrid intGrid;
    AABBi boundingBox;
};
