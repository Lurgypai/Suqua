#include <string>
#include <vector>
#include "StartCommand.h"
#include "DebugIO.h"

StartCommand::StartCommand(Game & game_) :
	game{game_}
{}

std::string StartCommand::getTag() const {
	return "startgame";
}

void StartCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() >= 2) {
		if (args[1] == "0") {
			game.startOfflineGame();
		}
		else if (args[1] == "1") {
			game.startOnlineGame();
		}
		else if (args[1] == "2") {
			if (args.size() == 3) {
				game.startStageEditor(args[2]);
			}
			else {
				DebugIO::printLine("Incorrect amount of args.");
			}
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
