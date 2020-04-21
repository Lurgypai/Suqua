#pragma once
#include "GLRenderer.h"
#include "RenderComponent.h"
#include "PositionComponent.h"

class RenderSystem {
public:
	void drawAll();
	void draw(RenderComponent& render);
};
