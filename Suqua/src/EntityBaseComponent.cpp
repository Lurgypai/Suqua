#include "EntityBaseComponent.h"

EntityBaseComponent::EntityBaseComponent(EntityId id_) :
	id{id_},
	isDead{false},
	isActive{true}
{}
