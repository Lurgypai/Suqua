#pragma once
#include "Command.h"

class KillCommand : public Command {
public:
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
};