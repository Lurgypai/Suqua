#include "InventoryItem.h"


InventoryItem::InventoryItem(const Item& item_, int count_) :
    item{item_},
	count{count_},
	heldPos{},
	angle{}
{}
