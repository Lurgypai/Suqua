#pragma once
#include "IDrawable.h"
#include "AABB.h"

class RectDrawable : public IDrawable {
public:


	// Inherited via IDrawable
	virtual void draw() override;

	virtual void setPos(Vec2f pos) override;

	virtual IDrawable* clone() override;

	float r;
	float g;
	float b;

	AABB shape;
};