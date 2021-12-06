#pragma once
#include "InputDevice.h"

class IDKeyboardMouse : public InputDevice {
public:
	IDKeyboardMouse(InputDeviceId id_);
	virtual void doInput(EntityId controllerCompId) override;
};