#pragma once
#include <cstdint>
#include "Vec2.h"
#include "ByteStream.h"

using ControllerState = uint32_t;

enum ControllerBits : ControllerState {
	UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,

	BUTTON_1 = 1 << 4, // circle
	BUTTON_2 = 1 << 5, // x
	BUTTON_3 = 1 << 6, // square
	BUTTON_4 = 1 << 7, // triangle

	BUTTON_5 = 1 << 8,      // l
	BUTTON_6 = 1 << 9,      // r
	BUTTON_7 = 1 << 10,     // l2
	BUTTON_8 = 1 << 11,     // r2
	BUTTON_9 = 1 << 12,     // l3
	BUTTON_10 = 1 << 13,    // r3
	BUTTON_11 = 1 << 14,    // select
	BUTTON_12 = 1 << 15,    // start

	NONE = 0
};

class Controller {
public:
	Controller();
	Controller(const Controller& other) = default;
	Controller(Controller&& other) = default;

	Controller& operator=(const Controller& other) = default;
	Controller& operator=(Controller&& other) = default;

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
    
    void storePrev();

	void serialize(ByteStream& data) const;
	void unserialize(ByteStream& data);

	Vec2i pointerPos;
	int mouseScroll;
	Vec2f stick1;
	Vec2f stick2;
private:
	ControllerState state;
	ControllerState prevState;
};
