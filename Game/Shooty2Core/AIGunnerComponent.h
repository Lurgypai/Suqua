#pragma once
#include "ComponentMacros.h"

#include <set>

#include "TeamComponent.h"

class AIGunnerComponent {
    CompMembers(AIGunnerComponent);
public:
	enum class AIState {
		idle,
		walking,
		following,
		attacking,
	};

    AIGunnerComponent(EntityId id_, float followRadius_, float avoidRadius_);

    void update(double delta);
    float followRadius;
    float avoidRadius;
    
    AIState getState() const;
private:
    double timeSinceLastAction;
    double idleTime;
    double walkTime;
    double followTime;
    double attackTime;

    EntityId targetId;
    std::set<TeamComponent::TeamId> targetTeams;
    AIState state;
};
