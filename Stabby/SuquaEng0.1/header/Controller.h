#pragma once
#include <cstdint>

enum ControllerBits : std::uint8_t {
	UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,

	BUTTON_1 = 1 << 4,
	BUTTON_2 = 1 << 5,
	BUTTON_3 = 1 << 6,
	BUTTON_4 = 1 << 7,

	ALL = UP | DOWN | LEFT | RIGHT | BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4
};

class Controller {
public:
	Controller();
	Controller(unsigned char state_);
	bool operator[](unsigned char ctrl_bits) const;
	void on(unsigned char ctrl_bits);
	void off(unsigned char ctrl_bits);
	void set(unsigned char ctrl_bits, bool newState);
	void setMouseScroll(int mouse_scroll_);
	int getMouseScroll();
	unsigned char getState() const;
private:
	unsigned char state;
	int mouseScroll;
};