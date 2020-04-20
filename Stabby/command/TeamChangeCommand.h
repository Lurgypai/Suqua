#pragma once
#include "Command.h"
#include "../client/Client.h"

class TeamChangeCommand : public Command {
public:
	TeamChangeCommand(Client* client_);

	// Inherited via Command
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Client* client;
};