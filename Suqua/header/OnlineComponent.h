#pragma once
#include "ComponentMacros.h"
#include "NetworkTypes.h"

class OnlineComponent {
	friend class OnlineSystem;
    CompMembers(OnlineComponent);
public:
	OnlineComponent(EntityId id_);
	NetworkId getNetId() const;

private:
	NetworkId netId;
};
