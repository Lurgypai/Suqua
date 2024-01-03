#pragma once

class PhysicsComponent;

// collision direction refers to the side that collides, so left means the left side of the AABB collided (moving left)
enum class CollisionDir {
    left,
    right,
    up,
    down,
};

class CollisionHandler {
public:
    virtual ~CollisionHandler() {};
    virtual void onCollide(PhysicsComponent& physicsComp, CollisionDir dir) = 0;
};
