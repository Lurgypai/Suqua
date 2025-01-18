#pragma once
#include "ComponentMacros.h"
#include "AABB.h"

class MenuTextBoxComponent {
	friend class Menu;
    CompMembers(MenuTextBoxComponent);

public:
	MenuTextBoxComponent(EntityId id_);

	void update(Vec2f mousePos, bool toggled_);
	void input(const std::string& input);
	void backspace();
	void enterText();
	void setCharLimit(unsigned int charLimit_);

	bool isActive();
	bool pollActivated();
	bool pollEnteredText(std::string& text);
	const AABB& getBoundingBox() const;
	std::string getActiveText();

	const std::string& getTag() const;
private:
	std::string tag;

	std::string textBuffer;
	std::string enteredText;
	AABB boundingBox;
	bool active;
	bool activated;
	bool textWasEntered;
	unsigned int charLimit;
};
