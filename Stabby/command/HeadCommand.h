#pragma once
#include "Command.h"
#include <vector>

class HeadCommand : public Command {
public:
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
};