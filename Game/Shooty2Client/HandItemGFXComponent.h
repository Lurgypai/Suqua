#pragma once
#include "ComponentMacros.h"
#include <vector>
#include <string>
#include "HandItemGFXSystem.h"

struct RenderItem {
	size_t spriteIndex;
	std::string renderTag;
};

class HandItemGFXComponent {
    CompMembers(HandItemGFXComponent);
public:
	HandItemGFXComponent(EntityId id_);

	void update(const HandItemGFXSystem& invItemGFXSys);
private:
	std::vector<RenderItem> items;
};
