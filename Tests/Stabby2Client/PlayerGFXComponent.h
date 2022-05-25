#pragma once
#include "EntitySystem.h"
#include "../Stabby2Core/PlayerComponent.h"

class PlayerGFXComponent {
public:
	PlayerGFXComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(int millis);

private:
	PlayerComponent::PlayerState prevState;
	PlayerComponent::PlayerAttack prevAttack;
	EntityId id;
};