#pragma once
#include "EntitySystem.h"
#include "NetworkTypes.h"

class OnlineComponent {

	friend class OnlineSystem;

public:
	OnlineComponent(EntityId id_ = 0);
	EntityId getId() const;
	NetworkId getNetId() const;

private:
	NetworkId netId;
	EntityId id;
};