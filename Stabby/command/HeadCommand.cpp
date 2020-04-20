#include <string>
#include "HeadCommand.h"
std::string HeadCommand::getTag() const {
	return "head";
}

void HeadCommand::onCommand(const std::vector<std::string>& args) {
}
