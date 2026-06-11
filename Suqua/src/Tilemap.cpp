#include "Tilemap.h"

Tilemap::Tilemap(const TilemapGrid& intGrid_, const AABBi& boundingBox_) :
    intGrid{intGrid_},
    boundingBox{boundingBox_}
{}

const AABBi& Tilemap::getBoundingBox() const {
    return boundingBox;
}

bool Tilemap::hasTile(const Vec2f& pos) const {
    Vec2i tilePos = pos - boundingBox.pos;
    return intGrid[tilePos.x][tilePos.y];
}

bool Tilemap::contains(const Vec2f& pos) const {
    return boundingBox.contains(pos);
}
