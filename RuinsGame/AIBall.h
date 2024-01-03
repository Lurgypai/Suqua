#pragma once
#include "Level.h"
#include "InputDevice.h"
#include "TeamComponent.h"

class AIBall : public InputDevice {
public:
	AIBall(InputDeviceId id_);
	virtual Controller getControllerState() override;
	virtual void update() override;

	// id of the entity that is controlled by this controller
	EntityId entityId;

    Vec2f left;
    Vec2f right;
    Vec2f downleft;
    Vec2f downright;

    Level* level;
private:
	// this needs a local copy of the controller so we can maintain a proper previous state
	// the controller is copied by the getControllerState function
	Controller controller;



    float dir;
};
