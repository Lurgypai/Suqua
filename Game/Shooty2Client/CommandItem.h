#pragma once
#include "Command.h"
#include "../Shooty2Core/ItemSystem.h"

class CommandItem : public Command {
public:
    CommandItem(ItemSystem& items_, EntityId playerId_);

    std::string getTag() const override;
    void onCommand(const std::vector<std::string>& args) override;
private:
    ItemSystem* items;
    EntityId playerId;
};
