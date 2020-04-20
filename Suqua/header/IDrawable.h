#pragma once
#include "Vec2.h"

class IDrawable {
public:
	virtual ~IDrawable() {};
	virtual void draw() = 0;
	virtual void setPos(Vec2f pos) = 0;

	virtual IDrawable* clone() = 0;
};