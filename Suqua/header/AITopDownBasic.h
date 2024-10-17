#pragma once
#include "InputDevice.h"
#include "TeamComponent.h"

class AITopDownBasic : public InputDevice {
public:
	AITopDownBasic(InputDeviceId id_);
	virtual Controller getControllerState() override;
	virtual void update() override;

	void setTargetTeams(std::vector<TeamComponent::TeamId> targetTeams_);

	enum class AIState {
		idle,
		walking,
		following,
		attacking,
	};
	
	// id of the entity that is controlled by this controller
	EntityId entityId;
	float followRadius;
    float avoidRadius;

	AIState getState() const;
private:
	// this needs a local copy of the controller so we can maintain a proper previous state
	// the controller is copied by the getControllerState function
	Controller controller;

	std::vector<TeamComponent::TeamId> targetTeams;
	EntityId targetId;
	AIState state;

	int stateTick;
	int walkTickMax;
	int idleTickMax;
	int followTickMax;
	int attackTickMax;

    bool moveLeft;

	EntityId findTarget();
};
