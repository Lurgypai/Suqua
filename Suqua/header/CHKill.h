#pragma once
#include "CollisionHandler.h"

class CHKill : public CollisionHandler {
public:
    ~CHKill() override;
    void onCollide(PhysicsComponent& physicsComp, CollisionDir dir) override;
};
