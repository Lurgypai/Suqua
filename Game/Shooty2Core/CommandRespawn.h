#pragma once
#include "Command.h"
#include "World.h"

class CommandRespawn : public Command {
public:
    CommandRespawn(World& world);

    std::string getTag() const override;
    void onCommand(const std::vector<std::string>& args) override;
private:
    World* world;
};
