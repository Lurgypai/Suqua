#include "MenuGridComponent.h"
#include "PositionComponent.h"
#include <iostream>

MenuGridComponent::MenuGridComponent(EntityId id_) :
	id{ id_ },
	wasActive{ false },
	isActive{ false },
	toggled{ false },
	prefix{}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

EntityId MenuGridComponent::getId() const {
	return id;
}

void MenuGridComponent::update(Vec2f mousePos, bool toggled_) {
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
	boundingBox.pos = pos->pos;

	wasActive = isActive;
	isActive = false;
	int columns = boundingBox.res.x / (margins.x + buttonRes.x); 
	int rows = boundingBox.res.y / (margins.y + buttonRes.y);
	if (boundingBox.contains(mousePos) && toggled_) {
		//clicked inside bounding box
		int i = 0;
		for (auto& button : buttons[prefix]) {
			Vec2i buttonPos = { i % columns, i / columns };
			if (buttonPos.y < rows) {
				AABB currBox = AABB{ {pos->pos.x + margins.x + buttonPos.x * (buttonRes.x + margins.x), pos->pos.y + margins.y + buttonPos.y * (buttonRes.y + margins.y)}, buttonRes };
				if (currBox.contains(mousePos)) {

					selected = prefix + button;

					isActive = true;
					if (isActive && !wasActive)
						toggled = true;
					break;
				}
			}
			++i;
		}
	}
}

bool MenuGridComponent::pollToggled() {
	bool ret = toggled;
	toggled = false;
	return ret;
}

const AABB& MenuGridComponent::getBoundingBox() const {
	return boundingBox;
}

void MenuGridComponent::setPrefix(const std::string& s) {
	prefix = s;
}

const std::string& MenuGridComponent::getSelected() const {
	return selected;
}

void MenuGridComponent::setButtons(std::vector<std::string> s) {
	buttons = s;
}

const StringTree& MenuGridComponent::getButtons() const {
	return buttons;
}

StringTree MenuGridComponent::getCurrButtons() const {
	return buttons[prefix];
}

std::vector<AABB> MenuGridComponent::generateButtonBoxes() {
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
	int columns = boundingBox.res.x / (margins.x + buttonRes.x);
	int rows = boundingBox.res.y / (margins.y + buttonRes.y);

	int i = 0;
	std::vector<AABB> boxes;
	for (auto& tag : buttons) {
		Vec2i buttonPos = { i % columns, i / columns };
		boxes.emplace_back( Vec2f{ pos->pos.x + margins.x + buttonPos.x * (buttonRes.x + margins.x), pos->pos.y + margins.y + buttonPos.y * (buttonRes.y + margins.y) }, buttonRes);
		++i;
	}

	return boxes;
}