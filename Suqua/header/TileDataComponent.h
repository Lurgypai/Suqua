#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class TileDataComponent {
    CompMembers(TileDataComponent);
public:
    TileDataComponent(EntityId id_,
            Vec2f texOffset_,
            std::string textureTag_);

    Vec2f texOffset;
    std::string textureTag;
private:
};
