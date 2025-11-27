#include "Item.h"

Item::Item(const std::string& displayName_, const std::string& tag_, const UseCallback& useCallback_) :
	displayName{displayName_},
	tag{tag_},
	useCallback{useCallback_}
{}

void Item::onUse() const {
	if (useCallback) useCallback();
}