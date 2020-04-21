#include "BitmapFont.h"
#include "Vec2.h"
#include "GLRenderer.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "glad/glad.h"
#include "SDL.h"
#include "stb_image.h"

BitmapFont::BitmapFont() : texture{0} {
}

BitmapFont::BitmapFont(const std::string & fontFile, const std::string & fontDataFile) {
	std::ifstream fontImageFile{ fontFile };
	if (!fontImageFile.is_open())
		throw std::exception{};
	fontImageFile.close();

	int imgW, imgH, numChannels;
	unsigned char* img = stbi_load(fontFile.c_str(), &imgW, &imgH, &numChannels, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imgW, imgH, 0, GL_RED, GL_UNSIGNED_BYTE, img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(img);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//read data
	std::ifstream file;
	file.open(fontDataFile);
	if (!file.is_open())
		throw std::exception{};
	std::stringstream data;
	data << file.rdbuf();
	file.close();

	//read
	char s[256];
	while (data.getline(s, 256)) {

		//remove whitespace
		std::vector<std::string> splitLine;
		std::string word{};
		bool wordFinished = false;
		for (char c : s) {
			if (c == '\0') {
				splitLine.push_back(word);
				word.clear();
				break;
			}

			if (c != ' ') {
				word += c;
				wordFinished = false;
			}

			else if (!wordFinished) {
				splitLine.push_back(word);
				word.clear();
				wordFinished = true;
			}
		}

		//read line
		if (splitLine[0] == "info") {}
		if (splitLine[0] == "common") {}
		if (splitLine[0] == "page") {}
		if (splitLine[0] == "chars") {}

		if (splitLine[0] == "char") {
			Character character;
			//for every string in splitLine
			for (int i = 1; i != splitLine.size(); i++) {
				std::string s = splitLine[i];
				std::string value;
				bool isReading = false;
				//read everything after the = sign
				for (char c : s) {
					if (!isReading && c == '=') {
						isReading = true;
					}
					else if (isReading) {
						value += c;
					}
				}
				//using its place in the vector, store its value
				switch (i) {
				case 1:
					character.id = std::stoi(value);
					break;
				case 2:
					character.pos.x = std::stoi(value);
					break;
				case 3:
					character.pos.y = std::stoi(value);
					break;
				case 4:
					character.res.x = std::stoi(value);
					break;
				case 5:
					character.res.y = std::stoi(value);
					break;
				case 6:
					character.offset.x = std::stoi(value);
					break;
				case 7:
					character.offset.y = std::stoi(value);
					break;
				case 8:
					character.advance = std::stoi(value);
					break;
				case 9:
					break;
				case 10:
					break;

				}
				value.clear();
			}
			//store the char
			if (character.id < 128) {
				chars.insert({ static_cast<unsigned char>(character.id), character });
			}
		}
		//clear for reuse
		splitLine.clear();
	}

	renderBufferId = GLRenderer::GenRenderStyleBuf(texture, 0);
}

void BitmapFont::loadFromFiles(const std::string & fontFile, const std::string & fontDataFile) {
	std::ifstream fontImageFile{ fontFile };
	if (!fontImageFile.is_open())
		throw std::exception{};
	fontImageFile.close();

	chars.clear();
	int imgW, imgH, numChannels;
	unsigned char* img = stbi_load(fontFile.c_str(), &imgW, &imgH, &numChannels, 0);
	
	if(texture == 0)
		glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imgW, imgH, 0, GL_RED, GL_UNSIGNED_BYTE, img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(img);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//read data
	std::ifstream file;
	file.open(fontDataFile);
	if (!file.is_open())
		throw std::exception{};
	std::stringstream data;
	data << file.rdbuf();
	file.close();

	//read
	char s[256];
	while (data.getline(s, 256)) {

		//remove whitespace
		std::vector<std::string> splitLine;
		std::string word{};
		bool wordFinished = false;
		for (char c : s) {
			if (c == '\0') {
				splitLine.push_back(word);
				word.clear();
				break;
			}

			if (c != ' ') {
				word += c;
				wordFinished = false;
			}

			else if (!wordFinished) {
				splitLine.push_back(word);
				word.clear();
				wordFinished = true;
			}
		}

		//read line
		if (splitLine[0] == "info") {}
		if (splitLine[0] == "common") {}
		if (splitLine[0] == "page") {}
		if (splitLine[0] == "chars") {}

		if (splitLine[0] == "char") {
			Character character;
			//for every string in splitLine
			for (int i = 1; i != splitLine.size(); i++) {
				std::string s = splitLine[i];
				std::string value;
				bool isReading = false;
				//read everything after the = sign
				for (char c : s) {
					if (!isReading && c == '=') {
						isReading = true;
					}
					else if (isReading) {
						value += c;
					}
				}
				//using its place in the vector, store its value
				switch (i) {
				case 1:
					character.id = std::stoi(value);
					break;
				case 2:
					character.pos.x = std::stoi(value);
					break;
				case 3:
					character.pos.y = std::stoi(value);
					break;
				case 4:
					character.res.x = std::stoi(value);
					break;
				case 5:
					character.res.y = std::stoi(value);
					break;
				case 6:
					character.offset.x = std::stoi(value);
					break;
				case 7:
					character.offset.y = std::stoi(value);
					break;
				case 8:
					character.advance = std::stoi(value);
					break;
				case 9:
					break;
				case 10:
					break;

				}
				value.clear();
			}
			//store the char
			if (character.id < 128) {
				chars.insert({ static_cast<unsigned char>(character.id), character });
			}
		}
		//clear for reuse
		splitLine.clear();
	}

	renderBufferId = GLRenderer::GenRenderStyleBuf(texture, 0);
}

Character BitmapFont::getCharacter(unsigned char c) const {
	if(c != '\0')
	return chars.at(c);
}

unsigned int BitmapFont::getTexture() const {
	return texture;
}

unsigned int BitmapFont::getRenderBuffer() const {
	return renderBufferId;
}
