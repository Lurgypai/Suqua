#pragma once
#include "CollisionHandler.h"

class TestCollisionHandler : public CollisionHandler {
public:
    ~TestCollisionHandler() override;
    void onCollide(PhysicsComponent& physicsComp, CollisionDir dir) override;
};
