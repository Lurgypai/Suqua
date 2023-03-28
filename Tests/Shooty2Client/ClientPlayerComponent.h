#pragma once
#include "EntitySystem.h"
#include "../Shooty2Core/PlayerComponent.h"

class ClientPlayerComponent {
public:
	ClientPlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	void updateGFX();
private:
	PlayerComponent::State prevPlayerState;
	float prevPlayerDir;
	int prevCardinalDir;
	EntityId id;
};