
#include "OnlineComponent.h"

OnlineComponent::OnlineComponent(EntityId id_) :
	id{id_}
{}

NetworkId OnlineComponent::getNetId() const {
	return netId;
}
