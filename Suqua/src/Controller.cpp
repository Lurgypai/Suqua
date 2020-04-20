#include "Controller.h"
#include <vector>


Controller::Controller() : state{ 0 }, mouseScroll{0} {}

Controller::Controller(unsigned char state_) :
	state{state_}
{}

bool Controller::operator[](unsigned char ctrl_bits) const {
	return state & ctrl_bits;
}

void Controller::on(unsigned char ctrl_bits) {
	state |= ctrl_bits;
}

void Controller::off(unsigned char ctrl_bits) {
	state &= ~(ctrl_bits);
}

void Controller::set(unsigned char ctrl_bits, bool newState) {
	if (newState)
		state |= ctrl_bits;
	else
		state &= ~(ctrl_bits);
}

void Controller::setMouseScroll(int mouse_scroll_) {
	mouseScroll = mouse_scroll_;
}

int Controller::getMouseScroll() {
	return mouseScroll;
}

unsigned char Controller::getState() const {
	return state;
}
