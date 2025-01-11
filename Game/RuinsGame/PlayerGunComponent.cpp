#include "PlayerGunComponent.h"

PlayerGunComponent::PlayerGunComponent(EntityId id_) : 
	id{id_}
{}

EntityId PlayerGunComponent::getId() const {
	return id;
}
