#include "MenuButtonComponent.h"
#include "PositionComponent.h"

MenuButtonComponent::MenuButtonComponent(EntityId id_) :
	id{id_},
	wasActive{false},
	isActive{false},
	toggled{false}
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

void MenuButtonComponent::update(Vec2f mousePos, bool toggled_) {
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
	boundingBox.pos = pos->pos;

	wasActive = isActive;
	if (boundingBox.contains(mousePos) && toggled_) {
		isActive = true;
		if (isActive && !wasActive)
			toggled = true;
	}
	else {
		isActive = false;
	}
}

bool MenuButtonComponent::pollToggled() {
	bool ret = toggled;
	toggled = false;
	return ret;
}

const AABB& MenuButtonComponent::getBoundingBox() const {
	return boundingBox;
}

const std::string& MenuButtonComponent::getTag() const {
	return tag;
}