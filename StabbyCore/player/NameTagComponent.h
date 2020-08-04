#pragma once
#include "EntitySystem.h"
#include "CharBuffer.h"

class NameTagComponent {
public:
	NameTagComponent(EntityId id_ = 0);
	
	EntityId getId() const;

	static constexpr size_t SIZE = 25;
	CharBuffer<SIZE> nameTag;
private:
	EntityId id;
};