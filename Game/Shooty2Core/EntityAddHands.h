#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Controller.h"

struct EntityAddHandsArgs {
    Vec2f bodyOffset;
    float armLength;
    ControllerBits leftHand;
    ControllerBits rightHand;
};

extern void EntityAddHands(EntityId id, const EntityAddHandsArgs& args);
