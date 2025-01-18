#pragma once
#include "ComponentMacros.h"
#include "NetworkDataComponent.h"

class PlayerGunComponent {
    CompMembers(PlayerGunComponent);
public:
	enum PlayerGunVars : NetworkDataComponent::DataId {
		playerGunDirVarId = 40,
	};

	PlayerGunComponent(EntityId id_);

	float getDir() const;
	void setDir();
private:
};
