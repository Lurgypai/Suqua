#pragma once
#include "ComponentMacros.h"

class GunGFXComponent {
    CompMembers(GunGFXComponent);
public:
	GunGFXComponent(EntityId id_);

	void update();
private:
};
