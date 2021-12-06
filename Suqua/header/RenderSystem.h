#pragma once
#include "GLRenderer.h"
#include "RenderComponent.h"

class RenderSystem {
public:
	void drawAll() const;
	void draw(RenderComponent& render) const;
};
