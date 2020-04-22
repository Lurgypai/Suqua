#pragma once

#include <string>
#include <deque>

#include "EntitySystem.h"
#include "MenuId.h"
#include "MenuSystem.h"
#include "AABB.h"
#include "MenuResult.h"

class Menu {

	friend class MenuSystem;

public:
	Menu();
	Menu(MenuId id);

	void addMenuEntry(MenuEntryType type, const std::string& entryTag_, const AABB& boundingBox);
	bool pollResult(MenuResult& result_);
	void updateMenuEntries(int camId);

	const std::vector<EntityId>& getButtons() const;

private:
	EntityId makeButton(const std::string& entryTag_, const AABB& boundingBox);
	EntityId makeTextBox(const std::string& entryTag_, const AABB& boundingBox);

	MenuId id;
	std::deque<MenuResult> results;
	std::vector<EntityId> buttons;
	std::vector<EntityId> text_boxes;

	bool prevButton1;
};