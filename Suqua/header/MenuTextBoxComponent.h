#pragma once
#include "EntitySystem.h"
#include "AABB.h"

class MenuTextBoxComponent {
	friend class Menu;

public:
	MenuTextBoxComponent(EntityId id_ = 0);
	EntityId getId() const;

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
	EntityId id;

	std::string tag;

	std::string textBuffer;
	std::string enteredText;
	AABB boundingBox;
	bool active;
	bool activated;
	bool textWasEntered;
	unsigned int charLimit;
};