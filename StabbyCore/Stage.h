#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include <string>

#define STAGE_WIDTH 1000
#define STAGE_HEIGHT 10

class Stage {
public:
	Stage();
	void loadGraphics(std::string filePath);
	EntityId getId();

private:
	Vec2f pos;
	EntityId id;
};