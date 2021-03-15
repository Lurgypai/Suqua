#include "Controller.h"
#include <vector>


Controller::Controller() : state{ 0 }, prevState{0}, mouseScroll{ 0 } {}

Controller::Controller(const Controller& other) :
	state{other.state},
	prevState{other.prevState},
	mouseScroll{other.mouseScroll}
{}

Controller::Controller(ControllerState state_, ControllerState prevState_) :
	prevState{prevState_},
	state{state_},
	mouseScroll{0}
{}

bool Controller::operator[](ControllerState ctrl_bits) const {
	return state & ctrl_bits;
}

bool Controller::toggled(ControllerState ctrl_bits) const {
	return (state & ctrl_bits) != (prevState & ctrl_bits);
}

void Controller::on(ControllerState ctrl_bits) {
	prevState = (prevState & ~ctrl_bits) | (state & ctrl_bits);
	state |= ctrl_bits;
}

void Controller::off(ControllerState ctrl_bits) {
	prevState = (prevState & ~ctrl_bits) | (state & ctrl_bits);
	state &= ~(ctrl_bits);
}

void Controller::set(ControllerState ctrl_bits, bool newState) {
	prevState = (prevState & ~ctrl_bits) | (state & ctrl_bits);
	if (newState)
		state |= ctrl_bits;
	else
		state &= ~(ctrl_bits);
}

void Controller::setState(ControllerState newState) {
	prevState = state;
	state = newState;
}

ControllerState Controller::getState() const {
	return state;
}

ControllerState Controller::getPrevState() const {
	return prevState;
}
