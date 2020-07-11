#include "RectDrawable.h"
#include "GLRenderer.h"
#include "Color.h"

void RectDrawable::draw() {
	Primitive p{ { shape.pos, {shape.pos.x + shape.res.x, shape.pos.y}, shape.pos + shape.res, {shape.pos.x, shape.pos.y + shape.res.y} }, depth, c };
	GLRenderer::DrawPrimitive(p);
}

void RectDrawable::setPos(Vec2f pos) {
	shape.pos = pos;
}

IDrawable* RectDrawable::clone() {
	return new RectDrawable(*this);
}
