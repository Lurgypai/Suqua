#pragma once
#include "Command.h"

class FrameByFrameCommand : public Command {
public:
	FrameByFrameCommand(bool & doFBF_);

	std::string getTag() const override;
	void onCommand(const std::vector<std::string>& args) override;

private:
	bool & doFBF;
};