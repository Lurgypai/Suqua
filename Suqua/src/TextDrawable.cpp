#include "TextDrawable.h"
#include "GLRenderer.h"

TextDrawable::TextDrawable() :
	font{},
	text{},
	r{},
	g{},
	b{},
	scale{ 1.0, 1.0 },
	anti_alias{ false }
{}

void TextDrawable::setPos(Vec2f pos) {
	boundingBox.pos = pos;
}

IDrawable* TextDrawable::clone() {
	return new TextDrawable(*this);
}

void TextDrawable::draw() {
	GLRenderer::SetDefShader(TextShader);
	GLRenderer::bindCurrShader();
	GLRenderer::GetDefaultShader(TextShader).uniform3f("color", r, g, b);
	GLRenderer::GetDefaultShader(TextShader).uniform1i("anti_alias", anti_alias);

	Vec2f displayOffset = boundingBox.pos;
	Vec2i pos = displayOffset;
	for (char c : text) {
		Character glyph = font.getCharacter(c);

		ImgData data{
			Vec2f{ pos.x + (static_cast<float>(glyph.offset.x) * scale.x), pos.y + (static_cast<float>(glyph.offset.y) * scale.y)},	//objpos
			Vec2f{512.0f, 256.0f},	//imgRes
			Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},	//imgOffset
			Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) },	//objres
			{0.0f, 0.0f},	//origin
			scale,	//scale
			0.0f	//angle
			-1.0f	//depth
		};
		GLRenderer::DrawImage(data, "test_font");
		pos.x += glyph.advance * scale.x;
	}
	pos.x = displayOffset.x;
}

void TextDrawable::setColor(float r_, float g_, float b_) {
	r = r_;
	g = g_;
	b = b_;
}

const AABB& TextDrawable::getBoundingBox() {
	boundingBox.res.x = 0;
	boundingBox.res.y = 0;

	for (auto& c : text) {
		Character glyph = font.getCharacter(c);
		boundingBox.res.x += glyph.advance * scale.x;
		float height = (glyph.offset.y + glyph.res.y) * scale.y;
		if (height > boundingBox.res.y)
			boundingBox.res.y = height;
	}
	return boundingBox;
}