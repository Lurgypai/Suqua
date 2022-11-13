#pragma once
#include "InputDevice.h"

class IDKeyboardMouse : public InputDevice {
public:
	IDKeyboardMouse(InputDeviceId id_);
	virtual Controller getControllerState() override;

private:
	// this needs a local copy of the controller so we can maintain a proper previous state
	// the controller is copied by the getControllerState function
	Controller controller;
};