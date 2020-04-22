#include "MenuButtonComponent.h"
#include "PositionComponent.h"

MenuButtonComponent::MenuButtonComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

EntityId MenuButtonComponent::getId() const {
	return id;
}

void MenuButtonComponent::update(Vec2f mousePos) {
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
	boundingBox.pos = pos->pos;

	wasActive = isActive;
	if (boundingBox.contains(mousePos)) {
		isActive = true;
		if (isActive && !wasActive)
			toggled = true;
	}
}

bool MenuButtonComponent::pollToggled() {
	bool ret = toggled;
	toggled = false;
	return ret;
}
