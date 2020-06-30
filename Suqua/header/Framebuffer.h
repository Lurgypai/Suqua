#pragma once
#include <vector>

#include "Texture.h"

class Framebuffer {
public:
	Framebuffer();
	void bind();
	//Returns where the texture is located in the framebuffer.
	size_t addTexture1D(int width, GLenum colorIn, GLenum colorOut, unsigned char *src, GLenum location);
	//Returns where the texture is located in the framebuffer.
	size_t addTexture2D(int width, int height, GLenum colorIn, GLenum colorOut, unsigned char* src, GLenum location);

	Texture getTexture(unsigned int i);

	void makeDepthBuffer(int width, int height);

	void finalizeFramebuffer();

	//const access to iterating over the textures
	auto getTextureBegin() const;
	auto getTextureEnd() const;

	static void unbind();
private:
	unsigned int id;
	unsigned int rbo; //for depth/stencil of the buffer
	std::vector<Texture> textures;
};