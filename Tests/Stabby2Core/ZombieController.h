#pragma once
#include "InputDevice.h"

class ZombieController : public InputDevice {
public:
	ZombieController(InputDeviceId id_, EntityId entity);
	virtual Controller getControllerState() override;
private:
	EntityId entity;
	static constexpr unsigned int followDist = 200;
	static constexpr unsigned int lungeDist = 100;
};