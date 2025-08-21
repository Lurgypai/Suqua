#include "TileDataComponent.h"

TileDataComponent::TileDataComponent(EntityId id_, Vec2f texOffset_, std::string textureTag_) :
    id{id_},
    texOffset{texOffset_},
    textureTag{textureTag_}
{}
