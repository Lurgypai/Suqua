#pragma once
#include "InputDevice.h"

class ZombieController : public InputDevice {
public:
	ZombieController(InputDeviceId id_);
	virtual void doInput(EntityId controllerCompId) override;
private:
	static constexpr unsigned int followDist = 200;
	static constexpr unsigned int lungeDist = 100;
};