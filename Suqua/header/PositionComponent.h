#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

class PositionComponent {
    CompMembers(PositionComponent);
public:
    PositionComponent(EntityId id_, const Vec2f& pos = Vec2f{});

    Vec2f pos;
private:
};
