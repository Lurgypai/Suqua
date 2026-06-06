#include "Item.h"


Item::Item(const std::string& tag_, bool doesStack_) :
	tag{tag_},
    doesStack{doesStack_},
	ability{nullptr}
{}

Item::Item(const Item& other) :
    tag{other.tag},
    doesStack{other.doesStack},
    ability{other.ability->clone()}
{}

Item& Item::operator=(const Item& other) {
    tag = other.tag;
    doesStack = other.doesStack;
    ability = other.ability->clone();
    return *this;
}

const std::string& Item::getTag() const {
    return tag;
}

bool Item::getDoesStack() const {
    return doesStack;
}

const ItemAbility* Item::getAbility() const {
    return ability.get();
}

ItemAbility* Item::getAbility() {
    return ability.get();
}
