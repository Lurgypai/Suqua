#pragma once
#include "Command.h"

class ExitCommand : public Command {
public:
    std::string getTag() const override;
    void onCommand(const std::vector<std::string>& args) override;
};
