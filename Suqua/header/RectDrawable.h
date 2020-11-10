#pragma once
#include "IDrawable.h"
#include "AABB.h"
#include "Color.h"

class RectDrawable : public IDrawable {
public:


	// Inherited via IDrawable
	virtual void draw() override;

	virtual void setPos(Vec2f pos) override;

	virtual IDrawable* clone() override;

	Color c;
	bool filled;
	float depth;
	AABB shape;
};