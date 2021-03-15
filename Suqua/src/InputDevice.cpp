#include "InputDevice.h"

InputDevice::InputDevice(InputDeviceId id_) :
	id{id_}
{}

InputDeviceId InputDevice::getId() const {
	return id;
}