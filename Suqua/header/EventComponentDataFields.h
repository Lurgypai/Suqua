#include "EventComponent.h"

namespace MovementEvents {
	constexpr EventComponent::EventId START_MOVING = 1;
	constexpr EventComponent::EventId STOP_MOVING = 2;
}

namespace AttacKEvents {
	constexpr EventComponent::EventId START_ATTACKING = 3;
	constexpr EventComponent::EventId STOP_ATTACKING = 4;
}