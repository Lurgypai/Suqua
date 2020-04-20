#pragma once
#include "Command.h"

class HealthCommand : public Command {
public:
	// Inherited via Command
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
};