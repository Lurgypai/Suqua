#include "PositionComponent.h"

PositionComponent::PositionComponent(EntityId id_, Vec2f pos_) : id{ id_ }, pos{pos_}
{}

EntityId PositionComponent::getId() const {
	return id;
}
