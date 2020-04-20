#include "PlayerStateComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"

PlayerStateComponent::PlayerStateComponent(EntityId id_) :
	id{id_}
{
	playerState = PlayerState();
}

EntityId PlayerStateComponent::getId() const {
	return id;
}
