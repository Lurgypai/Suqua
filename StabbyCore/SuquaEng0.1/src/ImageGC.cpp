#include "stdafx.h"
#include "ImageGC.h"

ImageGC::ImageGC(EntityId id_) :
	id{ id_ },
	sprite{"images/none.png"}
{}

EntityId ImageGC::getId() const {
	return id;
}

void ImageGC::loadImageFromFile(const std::string & filePath) {
	sprite = Sprite{ filePath };
}

Sprite & ImageGC::getSprite() {
	return sprite;
}

unsigned int ImageGC::getTexture() const {
	return sprite.getTexture();;
}

unsigned int ImageGC::getRenderBuffer() const {
	return sprite.getRenderBufferId();
}

ImgData ImageGC::getImgData() const {
	return sprite.getImgData();
}