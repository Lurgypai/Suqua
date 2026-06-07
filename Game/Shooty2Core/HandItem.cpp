#include "HandItem.h"


HandItem::HandItem(const Item& item_, int count_, EntityId targetEntity_) :
    item{item_},
    targetEntity{targetEntity_},
	heldPos{},
	angle{}
{}
