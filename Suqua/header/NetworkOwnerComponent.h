#pragma once
#include "EntitySystem.h"

class NetworkOwnerComponent {
public:
	enum class Owner {
		unassigned,
		local,
		foreign,
	};

	NetworkOwnerComponent(EntityId id_ = 0);
	EntityId getId() const;

	Owner owner;
private:
	EntityId id;
};