#include "Controller.h"
#include "ByteStream.h"
#include <vector>


Controller::Controller() : state{ 0 }, prevState{0}, mouseScroll{ 0 } {}

Controller::Controller(const Controller& other) :
	pointerPos{other.pointerPos},
	state{other.state},
	prevState{other.prevState},
	mouseScroll{other.mouseScroll}
{}

Controller::Controller(ControllerState state_, ControllerState prevState_) :
	prevState{prevState_},
	state{state_},
	mouseScroll{0}
{}

bool Controller::operator==(const Controller& other) const {
    return pointerPos == other.pointerPos && mouseScroll == other.mouseScroll && state == other.state && prevState == other.prevState;
}

bool Controller::operator!=(const Controller& other) const {
    return !((*this) == other);
}

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

void Controller::serialize(ByteStream& stream) const {
	stream << pointerPos.x;
	stream << pointerPos.y;
	stream << mouseScroll;
	stream << state;
	stream << prevState;
}

void Controller::unserialize(ByteStream& stream) {
	stream >> pointerPos.x;
	stream >> pointerPos.y;
	stream >> mouseScroll;
	stream >> state;
	stream >> prevState;
}