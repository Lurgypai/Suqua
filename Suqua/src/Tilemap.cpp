#include "Tilemap.h"

Tilemap::Tilemap(const Vec2i& tileRes_, const TilemapGrid& intGrid_, const AABBi& boundingBox_) :
    tileRes{ tileRes_ },
    intGrid{ intGrid_ },
    boundingBox{ boundingBox_ }
{}

const AABBi& Tilemap::getBoundingBox() const {
    return boundingBox;
}

bool Tilemap::hasTile(const Vec2f& pos) const {
	Vec2i tilePos = (pos - boundingBox.pos) / Vec2f{ tileRes.x, tileRes.y };
    return intGrid[tilePos.x][tilePos.y];
}

bool Tilemap::hasTileInMap(const Vec2i& tilePos) const {
    return intGrid[tilePos.x][tilePos.y];
}

bool Tilemap::contains(const Vec2f& pos) const {
    return boundingBox.contains(pos);
}

bool Tilemap::intersects(const AABB& other) const {
	AABB ourBox{ Vec2f{boundingBox.pos}, Vec2f{boundingBox.res} };
    return ourBox.intersects(other);
}

const Vec2i& Tilemap::getTileRes() const {
    return tileRes;
}

Vec2i Tilemap::getTilePos(const Vec2f& pos) const {
    return Vec2i{ (pos - boundingBox.pos) / Vec2f{ tileRes.x, tileRes.y } };
}
