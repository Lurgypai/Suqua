#include <cstdlib>
#include "ExitCommand.h"

std::string ExitCommand::getTag() const {
    return "exit";
}

void ExitCommand::onCommand(const std::vector<std::string>& args) {
    exit(0);
}
