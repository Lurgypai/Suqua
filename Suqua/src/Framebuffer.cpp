#include "glad/glad.h"
#include "Framebuffer.h"
#include "SDL.h"
#include "GLRenderer.h"

Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &id);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

size_t Framebuffer::addTexture1D(int width, GLenum colorIn, GLenum colorOut, unsigned char * src, GLenum location) {
	textures.push_back(createTexture1D(width, colorIn, colorOut, src));
	textures.back().attatchment_loc = location;
	glFramebufferTexture1D(GL_FRAMEBUFFER, textures.back().attatchment_loc, textures.back().dimensions, textures.back().id, 0);
	return textures.size() - 1;
}

size_t Framebuffer::addTexture2D(int width, int height, GLenum colorIn, GLenum colorOut, unsigned char * src, GLenum location) {
	textures.push_back(createTexture2D(width, height, colorIn, colorOut, src));
	textures.back().attatchment_loc = location;
	glFramebufferTexture2D(GL_FRAMEBUFFER, textures.back().attatchment_loc, textures.back().dimensions, textures.back().id, 0);
	return textures.size() - 1;
}

Texture Framebuffer::getTexture(unsigned int i) {
	return textures[i];
}

void Framebuffer::finalizeFramebuffer() {

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create framebuffer.");
	}

	std::vector<GLenum> locs;
	locs.reserve(textures.size());
	for (auto& texture : textures) {
		locs.push_back(texture.attatchment_loc);
	}

	glDrawBuffers(locs.size(), locs.data());
}

auto Framebuffer::getTextureBegin() const {
	return textures.begin();
}

auto Framebuffer::getTextureEnd() const {
	return textures.end();
}

void Framebuffer::makeDepthBuffer(int width, int height) {
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
