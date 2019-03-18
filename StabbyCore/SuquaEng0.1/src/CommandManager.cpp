#include "stdafx.h"
#include "CommandManager.h"
#include <algorithm>

bool CommandManager::parseCommand(std::string command) {
	if (command[0] == COMMAND_SYMBOL) {
		std::string tag = command.substr(1, command.find(' ') - 1);
		for (auto& commandRunner : commands) {
			if (commandRunner->getTag() == tag) {

				//split
				std::vector<std::string> args{};
				int wordCount = std::count(command.begin(), command.end(), ' ') + 1;
				args.reserve(wordCount);
				for (int i = 0; i != wordCount; ++i) {
					args.push_back(command.substr(0, command.find(' ')));
					command = command.substr(command.find(' ') + 1, command.length());
				}

				commandRunner->onCommand(args);
				return true;
			}
		}
	}
	return false;
}
