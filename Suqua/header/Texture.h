#pragma once
#include "glad/glad.h"
#include "Vec2.h"

struct Texture {
	unsigned int id;
	Vec2f res;
	GLenum attatchment_loc;
	GLenum dimensions;
};

Texture createTexture2D(int width, int height, GLenum colorIn, GLenum colorOut, unsigned char* src);
Texture createTexture1D(int width, GLenum colorIn, GLenum colorOut, unsigned char *src);