#pragma once
#include "ComponentMacros.h"
#include "../Shooty2Core/AIState.h"

class EnemyGFXComponent {
    CompMembers(EnemyGFXComponent);
public:
    EnemyGFXComponent(EntityId id_);
    void update();
private:
    AIState prevState;
};
