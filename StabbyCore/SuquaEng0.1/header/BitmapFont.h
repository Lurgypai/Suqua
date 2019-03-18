#pragma once
#include "Sprite.h"
#include <string>
#include "ImgData.h"

#include <map>


struct Character {
	int id;
	Vec2i pos;
	Vec2i res;
	Vec2i offset;
	int advance;
};

class BitmapFont  {
public:
	BitmapFont();
	BitmapFont(const std::string& fontFile, const std::string& fontDataFile);
	void loadFromFiles(const std::string& fontFile, const std::string& fontDataFile);
	Character getCharacter(unsigned char c) const;
	unsigned int getTexture() const;
	unsigned int getRenderBuffer() const;
private:
	std::map<unsigned char, Character> chars;
	unsigned int texture;
	unsigned int renderBufferId;
};