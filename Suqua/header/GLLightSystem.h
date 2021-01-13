#pragma once
#include "Shader.h"
#include "GLLightSource.h"
#include "Sprite.h"
#include "GLRenderer.h"

class GLLightSystem {
public:
	GLLightSystem(unsigned int viewWidth, unsigned int viewHeight);

	void castRays(GLLightSource& source, const Camera& cam, unsigned int targetTextureId);
	void makeShadows(GLLightSource& source, const Camera& cam, time_t tick);
	void applyShadows(unsigned int source, Framebuffer* target);
	void clearShadowMaps();
private:
	Framebuffer shadowMap1;
	Framebuffer shadowMap2;
	int currTargetMap;
	Shader raycaster;
	Shader shadowShader;
	Shader applyShadowShader;
};