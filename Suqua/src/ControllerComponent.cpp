#include "ControllerComponent.h"

ControllerComponent::ControllerComponent(EntityId id_) :
	id{id_}
{}

const EntityId ControllerComponent::getId() const {
	return id;
}

void ControllerComponent::setController(const Controller& cont) {
	controller = cont;
}

Controller& ControllerComponent::getController() {
	return controller;
}
