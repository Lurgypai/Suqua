#include "ControllerComponent.h"

ControllerComponent::ControllerComponent(EntityId id_) :
	id{id_}
{}

void ControllerComponent::setController(const Controller& cont) {
	controller = cont;
}

Controller& ControllerComponent::getController() {
	return controller;
}

const Controller& ControllerComponent::getController() const {
    return controller;
}
