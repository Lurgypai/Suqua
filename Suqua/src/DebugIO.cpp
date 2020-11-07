#include "DebugIO.h"
#include "ImgData.h"

#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <SDL.h>
#include "stb_image.h"

#include "GLRenderer.h"
#define CONSOLE_DISPLAY_SIZE 10

FontData DebugIO::font{};
std::vector<std::string> DebugIO::lines = std::vector<std::string>{};
std::deque<std::string> DebugIO::console = std::deque<std::string>{};
bool DebugIO::isOpen = false;
Vec2i DebugIO::displayOffset{ 8, 36 };
Vec2i DebugIO::consoleOffset{8, 1000};
int DebugIO::lineSpacing{ 36 };
std::string DebugIO::inputText{};
CommandManager DebugIO::command_manager{};

void DebugIO::startDebug(std::string fontFile, std::string readFile) {
	font.loadDataFile(readFile);
	lines.resize(10);
}

void DebugIO::openDebug() {
	isOpen = true;
}

void DebugIO::closeDebug() {
	inputText = "";
	isOpen = false;
}

void DebugIO::toggleDebug() {
	if (isOpen)
		closeDebug();
	else
		openDebug();
}

void DebugIO::addInput(std::string in_) {
	inputText += in_;
}

void DebugIO::backspace() {
	if(!inputText.empty())
		inputText.pop_back();
}

void DebugIO::enterInput() {
	if (inputText != "") {
		if (!command_manager.parseCommand(inputText)) {
			console.push_front(inputText);
			while (console.size() > CONSOLE_DISPLAY_SIZE) {
				console.pop_back();
			}
		}
		inputText.clear();
	}
}

void DebugIO::printLine(std::string line) {
	console.push_front(line);
	while (console.size() > CONSOLE_DISPLAY_SIZE) {
		console.pop_back();
	}
}

void DebugIO::setLine(int line, std::string txt) {
	if (lines.size() > line) {
		lines[line] = txt;
	}
}

void DebugIO::drawLines() {

	GLRenderer::SetDefShader(TextShader);

	GLRenderer::bindCurrShader();
	GLRenderer::GetDefaultShader(TextShader).uniform3f("color", 1, 1, 1);
	GLRenderer::GetDefaultShader(TextShader).uniform1i("anti_alias", 1);

	Vec2i pos = displayOffset;
	for (std::string line : lines) {
		for (char c : line) {
			Character glyph = font.getCharacter(c);

			ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
				Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
				Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, -1.0f };
			GLRenderer::DrawImage(data, "test_font");
			pos.x += glyph.advance;
		}
		pos.x = displayOffset.x;
		pos.y += lineSpacing;
	}

	if (isOpen) {
		pos = consoleOffset;
		for (auto& c : inputText) {
			Character glyph = font.getCharacter(c);

			ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
				Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
				Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, -1.0f };
			GLRenderer::DrawImage(data, "test_font");
			pos.x += glyph.advance;
		}
		pos.x = consoleOffset.x;
		pos.y -= lineSpacing;

		for (auto& line : console) {
			for (auto& c : line) {
				Character glyph = font.getCharacter(c);

				ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
					Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
					Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, -1.0f };
				GLRenderer::DrawImage(data, "test_font");
				pos.x += glyph.advance;
			}
			pos.x = consoleOffset.x;
			pos.y -= lineSpacing;
		}
	}
}

void DebugIO::drawDebugInfo() {
	Vec2i pos = displayOffset;
	for (std::string line : lines) {
		for (char c : line) {
			Character glyph = font.getCharacter(c);

			ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
				Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
				Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, -1.0f };
			GLRenderer::DrawImage(data, "test_font");
			pos.x += glyph.advance;
		}
		pos.x = displayOffset.x;
		pos.y += lineSpacing;
	}
}

void DebugIO::stopDebug() {
}

bool DebugIO::getOpen() {
	return isOpen;
}

std::string DebugIO::getInput() {
	return console.front();
}

CommandManager & DebugIO::getCommandManager() {
	return command_manager;
}


