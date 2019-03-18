#include "stdafx.h"
#include "DebugIO.h"
#include "ImgData.h"

#include <fstream>
#include <sstream>

#include <glad\glad.h>
#include <SDL.H>
#include "stb_image.h"

#include "GLRenderer.h"
#define CONSOLE_DISPLAY_SIZE 10

BitmapFont DebugIO::font{};
std::vector<std::string> DebugIO::lines = std::vector<std::string>{};
std::deque<std::string> DebugIO::console = std::deque<std::string>{};
bool DebugIO::isOpen = false;
Vec2i DebugIO::displayOffset{ 8, 36 };
Vec2i DebugIO::consoleOffset{8, 1000};
int DebugIO::lineSpacing{ 36 };
std::string DebugIO::inputText{};
CommandManager DebugIO::command_manager{};

void DebugIO::startDebug(std::string fontFile, std::string readFile) {
	font.loadFromFiles(fontFile, readFile);
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

void DebugIO::drawLines(int windowHeight) {

	if (isOpen) {
		unsigned int renderBuf = font.getRenderBuffer();
		GLRenderer::ClearRenderBufs(GLRenderer::include, 1, &renderBuf);
		GLRenderer::SetBuffer(renderBuf);
		Vec2i pos = displayOffset;
		for (std::string line : lines) {
			for (char c : line) {
				Character glyph = font.getCharacter(c);

				ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
					Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
					Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f };
				GLRenderer::Buffer(data);
				pos.x += glyph.advance;
			}
			pos.x = displayOffset.x;
			pos.y += lineSpacing;
		}

		pos = consoleOffset;
		for (auto& c : inputText) {
			Character glyph = font.getCharacter(c);

			ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
				Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
				Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f };
			GLRenderer::Buffer(data);
			pos.x += glyph.advance;
		}
		pos.x = consoleOffset.x;
		pos.y -= lineSpacing;

		for (auto& line : console) {
			for (auto& c : line) {
				Character glyph = font.getCharacter(c);

				ImgData data{ Vec2i{ pos.x + glyph.offset.x, pos.y + glyph.offset.y}, Vec2f{512.0f, 256.0f},
					Vec2f{static_cast<float>(glyph.pos.x), static_cast<float>(glyph.pos.y)},
					Vec2f{ static_cast<float>(glyph.res.x), static_cast<float>(glyph.res.y) }, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f };
				GLRenderer::Buffer(data);
				pos.x += glyph.advance;
			}
			pos.x = consoleOffset.x;
			pos.y -= lineSpacing;
		}
		GLRenderer::Draw(GLRenderer::include, 1, &renderBuf);
	}
}

void DebugIO::stopDebug() {
}

unsigned int DebugIO::getRenderBuffer() {
	return font.getRenderBuffer();
}

bool DebugIO::getOpen() {
	return isOpen;
}

CommandManager & DebugIO::getCommandManager() {
	return command_manager;
}


