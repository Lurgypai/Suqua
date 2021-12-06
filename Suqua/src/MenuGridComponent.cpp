#include "MenuGridComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"
#include <iostream>

using NDC = NetworkDataComponent;

MenuGridComponent::MenuGridComponent(EntityId id_) :
	id{ id_ },
	wasActive{ false },
	isActive{ false },
	toggled{ false },
	prefix{}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NDC>() || !EntitySystem::GetComp<NDC>(id)) {
			EntitySystem::MakeComps<NDC>(1, &id);
		}
	}
}

EntityId MenuGridComponent::getId() const {
	return id;
}

void MenuGridComponent::update(Vec2f mousePos, bool toggled_) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	Vec2f pos{data->get<float>(X), data->get<float>(Y)};
	boundingBox.pos = pos;

	wasActive = isActive;
	isActive = false;
	int columns = boundingBox.res.x / (margins.x + buttonRes.x); 
	int rows = boundingBox.res.y / (margins.y + buttonRes.y);
	if (boundingBox.contains(mousePos) && toggled_) {
		//clicked inside bounding box
		int i = 0;
		for (auto button : buttons[prefix]) {
			Vec2i buttonPos = { i % columns, i / columns };
			if (buttonPos.y < rows) {
				AABB currBox = AABB{ {pos.x + margins.x + buttonPos.x * (buttonRes.x + margins.x), pos.y + margins.y + buttonPos.y * (buttonRes.y + margins.y)}, buttonRes };
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
	NDC* data = EntitySystem::GetComp<NDC>(id);
	Vec2f pos{ data->get<float>(X), data->get<float>(Y)};
	int columns = boundingBox.res.x / (margins.x + buttonRes.x);
	int rows = boundingBox.res.y / (margins.y + buttonRes.y);

	int i = 0;
	std::vector<AABB> boxes;
	for (auto tag : buttons) {
		Vec2i buttonPos = { i % columns, i / columns };
		boxes.emplace_back( Vec2f{ pos.x + margins.x + buttonPos.x * (buttonRes.x + margins.x), pos.y + margins.y + buttonPos.y * (buttonRes.y + margins.y) }, buttonRes);
		++i;
	}

	return boxes;
}