#include "ControllerComponent.h"

ControllerComponent::ControllerComponent(EntityId id_) :
	id{id_}
{}

bool ControllerComponent::operator==(const ControllerComponent& other) const {
    return controller == other.controller && id == other.id;
}

bool ControllerComponent::operator!=(const ControllerComponent& other) const {
    return !((*this) == other);
}

const EntityId ControllerComponent::getId() const {
	return id;
}

void ControllerComponent::setController(const Controller& cont) {
	controller = cont;
}

Controller& ControllerComponent::getController() {
	return controller;
}

const Controller& ControllerComponent::getControllerConst() const {
    return controller;
}
