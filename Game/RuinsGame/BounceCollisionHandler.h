#pragma once
#include "CollisionHandler.h"

class BounceCollisionHandler : public CollisionHandler {
public:
    ~BounceCollisionHandler() override;
    void onCollide(PhysicsComponent& physicsComp, CollisionDir dir) override;
};
