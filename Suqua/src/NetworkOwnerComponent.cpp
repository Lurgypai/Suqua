#include "NetworkOwnerComponent.h"

NetworkOwnerComponent::NetworkOwnerComponent(EntityId id_) :
	id{id_},
	owner{Owner::unassigned}
{}

EntityId NetworkOwnerComponent::getId() const {
	return id;
}
