#include "MenuButtonComponent.h"
#include "PositionComponent.h"

MenuButtonComponent::MenuButtonComponent(EntityId id_, const AABB& boundingBox_, const std::string& tag_) :
	id{id_},
    boundingBox{boundingBox_},
    tag{tag_},
	wasActive{false},
	isActive{false},
	toggled{false}
{
    if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
        EntitySystem::MakeComps<PositionComponent>(1, &id, boundingBox_.pos);
    }
}

void MenuButtonComponent::update(Vec2f mousePos, bool toggled_) {
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	boundingBox.pos = posComp->getPos();

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
