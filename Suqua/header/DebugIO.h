#pragma once
#include <string>
#include <vector>
#include <deque>
#include <map>

#include "Shader.h"
#include "Vec2.h"
#include "FontData.h"
#include "CommandManager.h"

class DebugIO {
public:
	static void startDebug(std::string fontFile, std::string readFile);
	static void openDebug();
	static void closeDebug();
	static void toggleDebug();
	static void addInput(std::string in_);
	static void backspace();
	static void enterInput();
	static void printLine(std::string line);
	static void setLine(int line, std::string txt);
	static void drawLines();
	static void drawDebugInfo();
	static void stopDebug();
	static bool getOpen();
	static CommandManager& getCommandManager();
private:
	static CommandManager command_manager;
	static Vec2i displayOffset;
	static Vec2i consoleOffset;
	static int lineSpacing;

	static bool isOpen;
	static FontData font;
	static std::vector<std::string> lines;
	static std::deque<std::string> console;
	static std::string inputText;
};