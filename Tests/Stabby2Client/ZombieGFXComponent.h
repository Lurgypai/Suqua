#pragma once
#include "EntitySystem.h"
#include "../Stabby2Core/ZombieComponent.h"

class ZombieGFXComponent {
public:
	ZombieGFXComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(int millis);

private:
	ZombieComponent::ZombieState prevState;
	ZombieComponent::ZombieState prevAttack;
	EntityId id;
};