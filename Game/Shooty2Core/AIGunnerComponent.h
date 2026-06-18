#pragma once
#include "ComponentMacros.h"

#include <set>

#include "AIState.h"
#include "TeamComponent.h"
#include "ItemSystem.h"

class AIGunnerComponent {
    CompMembers(AIGunnerComponent);
public:

    AIGunnerComponent(EntityId id_, float followRadius_, float avoidRadius_);

    void update(double delta, const ItemSystem& items);
    float followRadius;
    float avoidRadius;
    
    AIState getState() const;
    AIState state;
private:
    double timeSinceLastAction;
    double idleTime;
    double walkTime;
    double followTime;
    double attackTime;

    float angleMod;

    EntityId targetId;
    std::set<TeamComponent::TeamId> targetTeams;
};
