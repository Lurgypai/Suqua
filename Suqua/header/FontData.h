#pragma once
#include <string>
#include <map>

#include "Vec2.h"

struct Character {
	int id;
	Vec2i pos;
	Vec2i res;
	Vec2i offset;
	int advance;
};

class FontData  {
public:
	FontData();
	FontData(const std::string& fontDataFile);
	void loadDataFile(const std::string& fontDataFile);
	Character getCharacter(unsigned char c) const;
private:
	std::map<unsigned char, Character> chars;
};
