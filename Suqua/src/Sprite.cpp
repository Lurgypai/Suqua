#include "Sprite.h"
#include "GLRenderer.h"
#include "FileNotFoundException.h"
#include "stb_image.h"
#include "glad/glad.h"
#include "SDL.h"

Sprite::Sprite(const std::string &filePath, Vec2f pos_, Vec2f origin_, Vec2f scale_, Vec2f imgOffset_, float angle_) :
	data{ pos_, {0.0f, 0.0f}, imgOffset_, {0.0f, 0.0f}, origin_, scale_, angle_ }
{
	int w, h;
	unsigned char* imgData = stbi_load(filePath.c_str(), &w, &h, &channels, 0);

	if (imgData == NULL) {
		FileNotFoundException e{ filePath };
		throw e;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(imgData);

	data.imgRes = { static_cast<float>(w), static_cast<float>(h) };
	data.objRes = { static_cast<float>(data.imgRes.x), static_cast<float>(data.imgRes.y) };

	renderBufferId = GLRenderer::GenRenderStyleBuf(texture, 0);
}

Sprite::~Sprite() {}

Vec2f Sprite::getImgRes() const {
	return data.imgRes;
}

Vec2f Sprite::getPos() const {
	return data.objPos;
}

Vec2f Sprite::getImgOffset() const {
	return data.imgOffset;
}

Vec2f Sprite::getObjRes() const {
	return data.objRes;
}

Vec2f Sprite::getOrigin() const {
	return data.origin;
}

Vec2f Sprite::getScale() const {
	return data.scale;
}

float Sprite::getAngle() const {
	return data.angle;
}

const ImgData & Sprite::getImgData() const {
	return data;
}

void Sprite::setPos(Vec2f pos_) {
	data.objPos = pos_;
}

void Sprite::setImgOffset(Vec2f imgOffset_) {
	data.imgOffset = imgOffset_;
}

void Sprite::setObjRes(Vec2f objRes_) {
	data.objRes = objRes_;
}

void Sprite::setOrigin(Vec2f origin_) {
	data.origin = origin_;
}

void Sprite::setScale(Vec2f scale_) {
	data.scale = scale_;
}

void Sprite::setAngle(float angle_) {
	data.angle = angle_;
}

int Sprite::getChannels() const {
	return channels;
}

unsigned int Sprite::getTexture() const {
	return texture;
}

unsigned int Sprite::getRenderBufferId() const {
	return renderBufferId;
}

void Sprite::draw() {
	GLRenderer::SetDefShader(ImageShader);
	GLRenderer::SetBuffer(renderBufferId);
	GLRenderer::Buffer(data);
	GLRenderer::Draw(GLRenderer::include, 1, &renderBufferId);
}

IDrawable* Sprite::clone() {
	return new Sprite(*this);
}
