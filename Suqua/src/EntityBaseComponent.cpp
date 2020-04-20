#include "EntityBaseComponent.h"

EntityBaseComponent::EntityBaseComponent(EntityId id_) :
	id{id_},
	isDead{false}
{}

const EntityId EntityBaseComponent::getId() const {
	return id;
}
