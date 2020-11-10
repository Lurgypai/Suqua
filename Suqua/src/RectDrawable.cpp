#include "RectDrawable.h"
#include "GLRenderer.h"
#include "Color.h"

void RectDrawable::draw() {
	if (!filled) {
		Primitive p{ { shape.pos, {shape.pos.x + shape.res.x, shape.pos.y}, shape.pos + shape.res, {shape.pos.x, shape.pos.y + shape.res.y} }, depth, c };
		GLRenderer::DrawPrimitive(p);
	}
	else {
		Primitive p{ { shape.pos, {shape.pos.x + shape.res.x, shape.pos.y}, {shape.pos.x, shape.pos.y + shape.res.y}, shape.pos + shape.res }, depth, c };
		GLRenderer::DrawFilledPrimitive(p);
	}
}

void RectDrawable::setPos(Vec2f pos) {
	shape.pos = pos;
}

IDrawable* RectDrawable::clone() {
	return new RectDrawable(*this);
}
