#pragma once
#include "Command.h"
#include <vector>
#include <memory>
#include <utility>
#define COMMAND_SYMBOL '/'

using command_ptr = std::unique_ptr<Command>;

//parses commands through individual command objects

class CommandManager {
public:
	template<typename T, typename U>
	void registerCommand(U&& args) {
		commands.push_back(std::make_unique<T>(std::forward<U>(args)));
	}

	template<typename T>
	void registerCommand() {
		commands.push_back(std::make_unique<T>());
	}

	bool parseCommand(std::string command);
private:
	std::vector<command_ptr> commands;
};