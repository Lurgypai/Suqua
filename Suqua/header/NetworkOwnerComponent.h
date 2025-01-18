#pragma once
#include "ComponentMacros.h"

class NetworkOwnerComponent {
    CompMembers(NetworkOwnerComponent);
public:
	enum class Owner {
		unassigned,
		local,
		foreign,
	};

	NetworkOwnerComponent(EntityId id_, Owner owner_ = Owner::unassigned);
	Owner owner;
private:
};
