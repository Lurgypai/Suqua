#include "Item.h"


Item::Item(const std::string& tag_) :
	tag{tag_},
	ability{nullptr}
{}

Item::Item(const Item& other) :
    tag{other.tag},
    ability{nullptr}
{
    if(other.ability) ability = other.ability->clone();
}

Item& Item::operator=(const Item& other) {
    tag = other.tag;
    if(other.ability) ability = other.ability->clone();
    return *this;
}

const std::string& Item::getTag() const {
    return tag;
}

const ItemAbility* Item::getAbility() const {
    return ability.get();
}

ItemAbility* Item::getAbility() {
    return ability.get();
}
