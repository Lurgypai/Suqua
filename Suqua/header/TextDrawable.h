#pragma once
#include "IDrawable.h"
#include "AABB.h"
#include "FontData.h"

class TextDrawable : public IDrawable {
public:
	TextDrawable();

	virtual void draw() override;

	virtual void setPos(Vec2f pos) override;

	virtual IDrawable* clone() override;

	void setColor(float r, float g, float b);

	const AABB& getBoundingBox();

	FontData font;
	std::string text;
	float r;
	float g;
	float b;
	Vec2f scale;
	int anti_alias;
private:
	AABB boundingBox;
};