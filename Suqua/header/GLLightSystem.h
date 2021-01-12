#pragma once
#include "Shader.h"
#include "GLLightSource.h"
#include "Sprite.h"
#include "GLRenderer.h"

class GLLightSystem {
public:
	GLLightSystem();

	void castRays(GLLightSource& source, const Camera& cam, unsigned int targetTextureId);
private:
	Shader raycaster;
};