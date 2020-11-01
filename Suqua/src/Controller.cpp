#include "Controller.h"
#include <vector>


Controller::Controller() : state{ 0 }, prevState{0}, mouseScroll{ 0 } {}

Controller::Controller(unsigned char state_, unsigned char prevState_) :
	prevState{prevState_},
	state{state_},
	mouseScroll{0}
{}

bool Controller::operator[](unsigned char ctrl_bits) const {
	return state & ctrl_bits;
}

bool Controller::toggled(unsigned char ctrl_bits) const {
	return (state & ctrl_bits) != (prevState & ctrl_bits);
}

void Controller::on(unsigned char ctrl_bits) {
	prevState = (prevState & ~ctrl_bits) | (state & ctrl_bits);
	state |= ctrl_bits;
}

void Controller::off(unsigned char ctrl_bits) {
	prevState = (prevState & ~ctrl_bits) | (state & ctrl_bits);
	state &= ~(ctrl_bits);
}

void Controller::set(unsigned char ctrl_bits, bool newState) {
	prevState = (prevState & ~ctrl_bits) | (state & ctrl_bits);
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

unsigned char Controller::getPrevState() const {
	return prevState;
}
