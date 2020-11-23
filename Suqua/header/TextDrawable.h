#pragma once
#include "IDrawable.h"
#include "AABB.h"
#include "FontData.h"
#include "Color.h"

class TextDrawable : public IDrawable {
public:
	TextDrawable();

	virtual void draw() override;

	virtual void setPos(Vec2f pos) override;

	virtual IDrawable* clone() override;

	const AABB& getBoundingBox();

	FontData font;
	std::string text;
	Color color;
	Vec2f scale;
	float depth;
	int anti_alias;
private:
	AABB boundingBox;
};