#include "InputDevice.h"

InputDevice::InputDevice(InputDeviceId id_) :
	id{id_}
{}

InputDevice::~InputDevice() {};

InputDeviceId InputDevice::getId() const {
	return id;
}
