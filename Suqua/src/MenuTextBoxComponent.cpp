#include "MenuTextBoxComponent.h"
#include "PositionComponent.h"

MenuTextBoxComponent::MenuTextBoxComponent(EntityId id_) :
	id{id_},
	active{false},
	activated{false},
	boundingBox{},
	textBuffer{},
	enteredText{},
	tag{},
	textWasEntered{false}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

EntityId MenuTextBoxComponent::getId() const {
	return id;
}

void MenuTextBoxComponent::update(Vec2f mousePos, bool toggled_) {
	bool wasActive = active;
	if (boundingBox.contains(mousePos) && toggled_) {
		active = true;
		if (!wasActive)
			activated = true;
	}
}

void MenuTextBoxComponent::input(const std::string& input) {
	textBuffer += input;
}

void MenuTextBoxComponent::backspace() {
	if (!textBuffer.empty()) {
		textBuffer.pop_back();
	}
}

void MenuTextBoxComponent::enterText() {
	enteredText = textBuffer;
	textBuffer.clear();
	active = false;
	textWasEntered = true;
}

bool MenuTextBoxComponent::isActive() {
	return active;
}

bool MenuTextBoxComponent::pollActivated() {
	bool ret = activated;
	activated = false;
	return ret;
}

bool MenuTextBoxComponent::pollEnteredText(std::string& text) {
	if (textWasEntered) {
		text = enteredText;
		textWasEntered = false;
		return true;
	}
	return false;
}

const AABB& MenuTextBoxComponent::getBoundingBox() const {
	return boundingBox;
}
std::string MenuTextBoxComponent::getActiveText() {
	return textBuffer;
}
const std::string& MenuTextBoxComponent::getTag() const {
	return tag;
}