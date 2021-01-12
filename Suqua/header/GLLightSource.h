#pragma once
#include "Vec2.h"
#include "Framebuffer.h"

class GLLightSource {
	friend class GLLightSystem;

public:
	GLLightSource(unsigned int radius_);
	~GLLightSource();

	Vec2f pos;

	unsigned int getTextureId() const;
	unsigned int getRadius() const;
	unsigned int getRayCount() const;
private:
	Framebuffer fb;
	unsigned int textureId;
	unsigned int rayCount;
	unsigned int radius;
};