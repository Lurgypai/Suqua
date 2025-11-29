#pragma once
#include "Command.h"
#include "World.h"
#include "../Shooty2Core/ItemSystem.h"

class CommandSetItem : public Command {
public:
    CommandSetItem(ItemSystem& items_, EntityId playerId_, EntityId daemonId_);

    std::string getTag() const override;
    void onCommand(const std::vector<std::string>& args) override;
private:
    ItemSystem* items;
    EntityId playerId;
    EntityId daemonId;
};