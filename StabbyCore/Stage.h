#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "AABB.h"
#include <string>

#define STAGE_WIDTH 1000
#define STAGE_HEIGHT 10

class Stage {
public:
	Stage();
	void loadGraphics(std::string filePath);
	const AABB& getCollider() const;
	EntityId getId();

private:
	AABB collider;
	Vec2f pos;
	EntityId id;
};