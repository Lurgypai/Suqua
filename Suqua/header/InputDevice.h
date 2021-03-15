#pragma once
#include "EntitySystem.h"
#include <memory>

using InputDeviceId = uint64_t;

class InputDevice {
public:
	InputDevice(InputDeviceId id_);
	virtual ~InputDevice() = 0 {};
	//reads input into the ControllerComponent
	virtual void doInput(EntityId controllerCompId) = 0;

	InputDeviceId getId() const;
private:
	InputDeviceId id;
};

using InputDevicePtr = std::unique_ptr<InputDevice>;