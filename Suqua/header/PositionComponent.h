#pragma once
#include "Vec2.h"
#include "EntitySystem.h"

class PositionComponent {
public:
	PositionComponent(EntityId id = 0, Vec2f pos = { 0.0f, 0.0f });
	EntityId getId() const;
	Vec2f pos;

private:
	EntityId id;
};