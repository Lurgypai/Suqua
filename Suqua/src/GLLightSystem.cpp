#include "GLLightSystem.h"

GLLightSystem::GLLightSystem() :
	raycaster{ "suqua/shaders/light_ray_cast.vert", "suqua/shaders/light_ray_cast.frag" }
{}

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
