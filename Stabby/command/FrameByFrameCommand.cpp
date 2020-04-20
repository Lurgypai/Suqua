#include <string>
#include <vector>
#include "FrameByFrameCommand.h"

FrameByFrameCommand::FrameByFrameCommand(bool & doFBF_) :
	doFBF{ doFBF_ }
{}

std::string FrameByFrameCommand::getTag() const {
	return "fbf";
}

void FrameByFrameCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 1)
		doFBF = true;
}
