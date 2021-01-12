#include "..\header\GLLightSource.h"
#include "glad/glad.h"

GLLightSource::GLLightSource(unsigned int radius_) :
	rayCount{1000},
	radius{radius_}
{
	fb.bind();
	textureId = fb.addTexture1D(rayCount, GL_RGBA, GL_RGBA, nullptr, GL_COLOR_ATTACHMENT0);
	fb.finalizeFramebuffer();
	Framebuffer::unbind();
}

GLLightSource::~GLLightSource() {}

unsigned int GLLightSource::getTextureId() const {
	return fb.getTexture(textureId).id;
}

unsigned int GLLightSource::getRadius() const {
	return radius;
}

unsigned int GLLightSource::getRayCount() const {
	return rayCount;
}