#pragma once
#include "ComponentMacros.h"

#include <string>
#include <map>

class InventoryComponent {
	CompMembers(InventoryComponent);
public:
    InventoryComponent(EntityId id_);
    bool hasItem(const std::string& item) const;

    void setItemCount(const std::string& item, int count);
    int getItemCount(const std::string& item) const;
    void useItem(const std::string& item);

    std::size_t getUniqueItemCount() const;

    std::map<std::string, int> items;
private:
};
