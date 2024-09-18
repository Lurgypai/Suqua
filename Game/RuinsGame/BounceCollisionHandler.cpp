#include "BounceCollisionHandler.h"
#include "PhysicsComponent.h"

BounceCollisionHandler::~BounceCollisionHandler() {};

void BounceCollisionHandler::onCollide(PhysicsComponent& physicsComp, CollisionDir dir) {
    auto vel = physicsComp.getVel();
    switch(dir) {
        case CollisionDir::left:
        case CollisionDir::right:
            vel.x *= -1;
            physicsComp.setVel(vel);
            break;
        case CollisionDir::up:
        case CollisionDir::down:
            vel.y *= -1;
            physicsComp.setVel(vel);
            break;
    }
}
