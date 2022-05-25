#include "RectDrawable.h"
#include "GLRenderer.h"
#include "Color.h"

void RectDrawable::draw() {
	if (!filled) {
		Primitive p{ { shape.pos, {shape.pos.x + shape.res.x - 1, shape.pos.y}, shape.pos + shape.res - Vec2f{1, 1}, {shape.pos.x, shape.pos.y + shape.res.y - 1}}, depth, c};
		GLRenderer::DrawPrimitive(p);
	}
	else {
		Primitive p{ { shape.pos, {shape.pos.x + shape.res.x - 1, shape.pos.y}, {shape.pos.x, shape.pos.y + shape.res.y - 1}, shape.pos + shape.res - Vec2f{1, 1} }, depth, c };
		GLRenderer::DrawFilledPrimitive(p);
	}
}

void RectDrawable::setPos(Vec2f pos) {
	shape.pos = pos;
}

IDrawable* RectDrawable::clone() {
	return new RectDrawable(*this);
}

RectDrawable::RectDrawable(Color c_, bool filled_, float depth_, const AABB& shape_) {
	c = c_;
	filled = filled_;
	depth = depth_;
	shape = shape_;
}
