#include "Sprite.h"
#include "GLRenderer.h"
#include "FileNotFoundException.h"
#include "stb_image.h"
#include "glad/glad.h"
#include "SDL.h"

Sprite::Sprite(const std::string &texture_tag_) :
	texture_tag{texture_tag_},
	data{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1.0, 1.0}, 0, 0 },
	horizontalFlip{false},
	verticalFlip{false}
{
	auto textureData = GLRenderer::GetTextureData(texture_tag);
	data.objRes = textureData.res;
	data.imgRes = textureData.res;
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

float Sprite::getDepth() const {
	return data.depth;
}

const ImgData & Sprite::getImgData() const {
	return data;
}

void Sprite::setImgRes(Vec2f res_) {
	data.imgRes = res_;
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

void Sprite::setDepth(float depth_) {
	data.depth = depth_;
}

int Sprite::getChannels() const {
	return channels;
}

void Sprite::draw() {
	int mult = horizontalFlip ? -1 : 1;
	data.objRes.x = std::abs(data.objRes.x) * mult;
	mult = verticalFlip ? -1 : 1;
	data.objRes.y = std::abs(data.objRes.y) * mult;
	GLRenderer::DrawImage(data, texture_tag);
}

IDrawable* Sprite::clone() {
	return new Sprite(*this);
}
