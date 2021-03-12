#pragma once
#include "GLRenderer.h"
#include "RenderComponent.h"

class RenderSystem {
public:
	void drawAll();
	void draw(RenderComponent& render);
};
