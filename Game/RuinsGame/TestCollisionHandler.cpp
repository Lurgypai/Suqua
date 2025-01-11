#include "TestCollisionHandler.h"
#include <iostream>

TestCollisionHandler::~TestCollisionHandler() {};

void TestCollisionHandler::onCollide(PhysicsComponent& physicsComp, CollisionDir dir) {
    switch (dir) {
    case CollisionDir::up:
        std::cout << "up\n";
        break;
    case CollisionDir::down:
        std::cout << "down\n";
        break;
    case CollisionDir::left:
        std::cout << "left\n";
        break;
    case CollisionDir::right:
        std::cout << "right\n";
        break;
        
    }
}
