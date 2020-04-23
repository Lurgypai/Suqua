#include "FontData.h"
#include "Vec2.h"
#include "GLRenderer.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "glad/glad.h"
#include "SDL.h"
#include "stb_image.h"

FontData::FontData() {
}

FontData::FontData(const std::string & fontDataFile) {
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
}

void FontData::loadDataFile(const std::string & fontDataFile) {
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
}

Character FontData::getCharacter(unsigned char c) const {
	if(c != '\0')
	return chars.at(c);
}
