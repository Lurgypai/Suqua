#include "pch.h"
#include "ConnectCommand.h"
#include "DebugIO.h"

ConnectCommand::ConnectCommand(Client & client_, Time_t & now_) : client{ &client_ }, time{&now_} {}

std::string ConnectCommand::getTag() const {
	return "connect";
}

void ConnectCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 3) {
		try {
			int port = std::stoi(args[2]);
			client->connect(*time, args[1], port);
		}
		catch (std::invalid_argument e) {
			DebugIO::printLine("Invalid argument.");
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
