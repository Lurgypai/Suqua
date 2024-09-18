#pragma once
#include "EntitySystem.h"
#include "NetworkDataComponent.h"

class PlayerGunComponent {
public:
	enum PlayerGunVars : NetworkDataComponent::DataId {
		playerGunDirVarId = 40,
	};

	PlayerGunComponent(EntityId id_ = 0);
	EntityId getId() const;

	float getDir() const;
	void setDir();
private:
	EntityId id;
};