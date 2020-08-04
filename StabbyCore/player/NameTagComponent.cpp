#include "NameTagComponent.h"

NameTagComponent::NameTagComponent(EntityId id_) :
	id{ id_ }
{}

EntityId NameTagComponent::getId() const {
	return id;
}