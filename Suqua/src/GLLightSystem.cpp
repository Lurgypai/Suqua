#include "GLLightSystem.h"

GLLightSystem::GLLightSystem(unsigned int viewWidth, unsigned int viewHeight) :
	raycaster{ "suqua/shaders/light_ray_cast.vert", "suqua/shaders/light_ray_cast.frag" },
	shadowShader{ "suqua/shaders/basic.vert", "suqua/shaders/shadow.frag" },
	applyShadowShader{ "suqua/shaders/basic.vert", "suqua/shaders/apply_shadow.frag" },
	currTargetMap{1}
{
	shadowMap1.bind();
	shadowMap1.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, nullptr, GL_COLOR_ATTACHMENT0);
	shadowMap1.finalizeFramebuffer();
	Framebuffer::unbind();

	shadowMap2.bind();
	shadowMap2.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, nullptr, GL_COLOR_ATTACHMENT0);
	shadowMap2.finalizeFramebuffer();
	Framebuffer::unbind();
}

void GLLightSystem::castRays(GLLightSource& source, const Camera& cam, unsigned int targetTextureId){

	glViewport(0, 0, source.rayCount, cam.res.y);
	source.fb.bind();
	GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
	raycaster.use();
	raycaster.uniform2f("camPos", cam.pos.x, cam.pos.y);
	raycaster.uniform2f("camRes", cam.res.x, cam.res.y);

	raycaster.uniform2f("lightPos", source.pos.x, source.pos.y);
	raycaster.uniform1ui("rayCount", source.rayCount);
	raycaster.uniform1ui("radius", source.radius);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, targetTextureId);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	Framebuffer::unbind();
}

void GLLightSystem::makeShadows(GLLightSource& source, const Camera& cam, time_t tick) {

	glViewport(0, 0, cam.res.x, cam.res.y);
	if (currTargetMap == 1) {
		shadowMap1.bind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap2.getTexture(0).id);
	}
	else {
		shadowMap2.bind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap1.getTexture(0).id);
	}

	shadowShader.use();
	shadowShader.uniform2f("camPos", cam.pos.x, cam.pos.y);
	shadowShader.uniform2f("camRes", cam.res.x, cam.res.y);
	shadowShader.uniform2f("lightPos", source.pos.x, source.pos.y);
	shadowShader.uniform1ui("radius", source.getRadius());
	shadowShader.uniform1ui("rayCount", source.getRayCount());
	shadowShader.uniform1ui("tick", tick);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, source.getTextureId());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	Framebuffer::unbind();

	currTargetMap = (currTargetMap % 2) + 1;
}

void GLLightSystem::applyShadows(unsigned int source, Framebuffer* target) {
	if (target)
		target->bind();
	else
		Framebuffer::unbind();

	GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto textureRes = shadowMap1.getTexture(0).res;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap1.getTexture(0).id);
	if (currTargetMap == 1) {
		textureRes = shadowMap2.getTexture(0).res;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap2.getTexture(0).id);
	}

	glViewport(0, 0, static_cast<unsigned int>(textureRes.x), static_cast<unsigned int>(textureRes.y));
	applyShadowShader.use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, source);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLLightSystem::clearShadowMaps() {
	shadowMap1.bind();
	GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shadowMap2.bind();
	GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}