
#include "OnlineComponent.h"

OnlineComponent::OnlineComponent(EntityId id_) :
	id{id_}
{}

EntityId OnlineComponent::getId() const {
	return id;
}

NetworkId OnlineComponent::getNetId() const {
	return netId;
}