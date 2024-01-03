#include "AIBall.h"
#include "PositionComponent.h"

AIBall::AIBall(InputDeviceId id_) :
    InputDevice{ id_ },
    controller{},
    dir{1.f},
    level{nullptr}
{}

Controller AIBall::getControllerState() {
    return controller;
}

void AIBall::update() {
    // get the entity
    // check the map to see if there is a tile in front
    // if not turn around

    auto position = EntitySystem::GetComp<PositionComponent>(entityId)->getPos();

    if(level != nullptr) {
        if(dir > 0.f) {
            if(level->hasTile(position + right) || !level->hasTile(position + downright)) {
                dir = -1.f;
            }
        }
        else {
            if(level->hasTile(position + left) || !level->hasTile(position + downleft)) {
                dir = 1.f;
            }
        }
    }
    else {
        std::cout << "ENEMY AI MISSING LEVEL DATA!!!\n";
    }
    
    controller.stick1 = {dir, 0.f};
}
