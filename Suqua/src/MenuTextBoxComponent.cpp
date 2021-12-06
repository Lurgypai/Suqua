#include "MenuTextBoxComponent.h"
#include "NetworkDataComponent.h"

using NDC = NetworkDataComponent;

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
		if (!EntitySystem::Contains<NDC>() || !EntitySystem::GetComp<NDC>(id)) {
			EntitySystem::MakeComps<NDC>(1, &id);
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
	if(textBuffer.size() != charLimit)
		textBuffer += input;
	if (textBuffer.size() > charLimit) {
		textBuffer = textBuffer.substr(0, charLimit);
	}
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

void MenuTextBoxComponent::setCharLimit(unsigned int charLimit_) {
	charLimit = charLimit_;
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