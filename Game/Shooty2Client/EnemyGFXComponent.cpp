#include "EnemyGFXComponent.h"

#include "CharacterGFXComponent.h"

#include "../Shooty2Core/AIGunnerComponent.h"

EnemyGFXComponent::EnemyGFXComponent(EntityId id_) :
    id{id_},
    prevState{ AIState::idle }
{ }

void EnemyGFXComponent::update() {
    AIGunnerComponent* ai = EntitySystem::GetComp<AIGunnerComponent>(id);
    CharacterGFXComponent* gfx = EntitySystem::GetComp<CharacterGFXComponent>(id);
    AIState curState = ai->getState();
    if(curState == prevState) return;
    switch(curState) {
        case AIState::attacking:
            gfx->playAnimation("attack", false);
            break;
        default:
            gfx->stopAnimation();
            break;
    }
    prevState = curState;
}
