#include "ConnectCommand.h"
#include "DebugIO.h"

ConnectCommand::ConnectCommand(Client & client_) : client{ &client_ } {}

std::string ConnectCommand::getTag() const {
	return "connect";
}

void ConnectCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 3) {
		try {
			int port = std::stoi(args[2]);
			client->connect(args[1], port);
		}
		catch (std::invalid_argument e) {
			DebugIO::printLine("Invalid argument.");
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
