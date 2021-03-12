#include "MenuButtonComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"

using NDC = NetworkDataComponent;

MenuButtonComponent::MenuButtonComponent(EntityId id_) :
	id{id_},
	wasActive{false},
	isActive{false},
	toggled{false}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NDC>() || !EntitySystem::GetComp<NDC>(id)) {
			EntitySystem::MakeComps<NDC>(1, &id);
		}
	}
}

EntityId MenuButtonComponent::getId() const {
	return id;
}

void MenuButtonComponent::update(Vec2f mousePos, bool toggled_) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	Vec2f pos{ data->get<float>(X), data->get<float>(Y) };
	boundingBox.pos = pos;

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
