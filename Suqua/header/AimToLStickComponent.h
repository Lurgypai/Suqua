#pragma once

#include "ComponentMacros.h"

class AimToLStickComponent {
	CompMembers(AimToLStickComponent);
public:
    AimToLStickComponent(EntityId id);
	void update();
private:
};
