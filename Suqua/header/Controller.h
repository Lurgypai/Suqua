#pragma once
#include <cstdint>
#include "Vec2.h"

using ControllerState = uint32_t;

enum ControllerBits : ControllerState {
	UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,

	BUTTON_1 = 1 << 4,
	BUTTON_2 = 1 << 5,
	BUTTON_3 = 1 << 6,
	BUTTON_4 = 1 << 7,

	BUTTON_5 = 1 << 8,
	BUTTON_6 = 1 << 9,
	BUTTON_7 = 1 << 10,
	BUTTON_8 = 1 << 11,
	BUTTON_9 = 1 << 12,
	BUTTON_10 = 1 << 13,
	BUTTON_11 = 1 << 14,
	BUTTON_12 = 1 << 15,

	ALL = UP | DOWN | LEFT | RIGHT | BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4,
	NONE = 0
};

class Controller {
public:
	Controller();
	Controller(const Controller& other);
	Controller(ControllerState state_, ControllerState prevState_);

    bool operator==(const Controller& other) const;
    bool operator!=(const Controller& other) const;
	bool operator[](ControllerState ctrl_bits) const;
	bool toggled(ControllerState ctrl_bits) const;

	//these all store the previous state
	void on(ControllerState ctrl_bits);
	void off(ControllerState ctrl_bits);
	void set(ControllerState ctrl_bits, bool newState);
	void setState(ControllerState newState);

	ControllerState getState() const;
	ControllerState getPrevState() const;

	Vec2i pointerPos;
	int mouseScroll;
private:
	ControllerState state;
	ControllerState prevState;
};
