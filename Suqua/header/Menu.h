#pragma once

#include <string>
#include <deque>

#include "EntitySystem.h"
#include "MenuId.h"
#include "AABB.h"
#include "MenuResult.h"

class Menu {

	friend class MenuSystem;

public:
	Menu();
	Menu(MenuId id);

	void addMenuEntry(MenuEntryType type, const std::string& entryTag_, const AABB& boundingBox);
	EntityId getMenuEntry(const std::string& entryTag_);
	bool pollResult(MenuResult& result_);
	void updateMenuEntries(int camId);
	void input(const std::string& text);
	void backspace();
	void enterText();

	void clear();

	const std::vector<EntityId>& getButtons() const;
	const std::vector<EntityId>& getTextBoxes() const;
	const std::vector<EntityId>& getGrids() const;

private:
	EntityId makeButton(const std::string& entryTag_, const AABB& boundingBox);
	EntityId makeTextBox(const std::string& entryTag_, const AABB& boundingBox);
	EntityId makeGrid(const std::string& entryTag_, const AABB& boundingBox);

	MenuId id;
	std::deque<MenuResult> results;
	std::vector<EntityId> buttons;
	std::vector<EntityId> text_boxes;
	std::vector<EntityId> grids;

	bool prevButton1;
};
