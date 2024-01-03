#include "AIBall.h"
#include "PositionComponent.h"
#include "ControllerComponent.h"

AIBallComponent::AIBallComponent(EntityId id_) :
    id{id_},
    dir{1.f},
    level{nullptr}
{}


void AIBallComponent::update() {
    // get the entity
    // check the map to see if there is a tile in front
    // if not turn around

    auto position = EntitySystem::GetComp<PositionComponent>(id)->getPos();

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
    
    auto contComp = EntitySystem::GetComp<ControllerComponent>(id);
    contComp->getController().stick1 = {dir, 0.f};
}
