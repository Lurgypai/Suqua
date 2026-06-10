#include "InventoryComponent.h"

InventoryComponent::InventoryComponent(EntityId id_) :
    id{id_}
{}

bool InventoryComponent::hasItem(const std::string& item) const {
    return items.contains(item);
}

void InventoryComponent::setItemCount(const std::string& item, int count) {
    if(count > 0) items[item] = count;
}

int InventoryComponent::getItemCount(const std::string& item) const {
    return items.at(item);
}

void InventoryComponent::useItem(const std::string& item) {
    auto iter = items.find(item);
    if(iter == items.end()) throw std::exception{};
    --iter->second;
    if(iter->second < 1) items.erase(iter);
}

std::size_t InventoryComponent::getUniqueItemCount() const {
    return items.size();
}
