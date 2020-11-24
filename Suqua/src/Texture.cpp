#include "Texture.h"
Texture createTexture2D(int width, int height, GLenum colorIn, GLenum colorOut, unsigned char* src) {
	Texture texture{};
	texture.attatchment_loc = 0;
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, colorOut, width, height, 0, colorIn, GL_UNSIGNED_BYTE, src);

	glBindTexture(GL_TEXTURE_2D, 0);
	texture.dimensions = GL_TEXTURE_2D;
	texture.res = Vec2f{ static_cast<float>(width), static_cast<float>(height) };
	return texture;
}

Texture createTexture1D(int width, GLenum colorIn, GLenum colorOut, unsigned char *src) {

	Texture texture{};
	texture.attatchment_loc = 0;
	glGenTextures(1, &texture.id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texture.id);

	glTexImage1D(GL_TEXTURE_1D, 0, colorOut, width, 0, colorIn, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture.dimensions = GL_TEXTURE_1D;
	return texture;
}