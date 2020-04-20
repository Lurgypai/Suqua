#pragma once
#include "Command.h"
#include "../client/Client.h"
class ConnectCommand : public Command {
public:
	// Inherited via Command
	ConnectCommand(Client& client);
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Client * client;
};