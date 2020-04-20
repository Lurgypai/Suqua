#include "DirectionComponent.h"

DirectionComponent::DirectionComponent(EntityId id_) :
	id{id_}
{}

EntityId DirectionComponent::getId() const {
	return id;
}
