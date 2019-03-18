#pragma once
#include <iostream>
#include <unordered_map>
#include "Sprite.h"
#include "Vec2.h"
#include "EntitySystem.h"

class ImageGC {
public:
	ImageGC(EntityId id_ = 0);

	EntityId getId() const;

	void loadImageFromFile(const std::string& filePath);
	Sprite& getSprite();
	unsigned int getTexture() const;
	unsigned int getRenderBuffer() const;
	ImgData getImgData() const;

	Sprite sprite;

private:
	EntityId id;
};