#include <cstring>

#include "SDL.h"

#include "Menu.h"
#include "MenuButtonComponent.h"
#include "MenuTextBoxComponent.h"
#include "MenuGridComponent.h"
#include "RenderComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"
#include "RectDrawable.h"
#include "GLRenderer.h"
#include "DebugIO.h"

using NDC = NetworkDataComponent;

Menu::Menu()
{}

Menu::Menu(MenuId id_) :
	id{id_},
	results{},
	buttons{},
	text_boxes{},

	prevButton1{false}
{}

void Menu::addMenuEntry(MenuEntryType type, const std::string& entryTag_, const AABB& boundingBox) {
	switch (type) {
	case MenuEntryType::button:
		makeButton(entryTag_, boundingBox);
		break;
	case MenuEntryType::text_box:
		makeTextBox(entryTag_, boundingBox);
		break;
	case MenuEntryType::grid:
		makeGrid(entryTag_, boundingBox);
	}
}

bool Menu::pollResult(MenuResult& result_) {
	if (results.empty()) {
		result_.type = MenuEntryType::none;
		return false;
	}
	else {
		result_ = results.front();
		results.pop_front();
		return true;
	}
}

void Menu::updateMenuEntries(int camId) {

	Vec2i mousePos_;
	Uint32 mouseState = SDL_GetMouseState(&mousePos_.x, &mousePos_.y);
	Vec2f mousePos = GLRenderer::screenToWorld(mousePos_, camId);

	bool currButton1 = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool toggled = false;

	if (currButton1 && !prevButton1)
		toggled = true;

	for (auto& buttonId : buttons) {
		MenuButtonComponent* button = EntitySystem::GetComp<MenuButtonComponent>(buttonId);
		button->update(mousePos, toggled);
		if (button->pollToggled()) {
			MenuResult r{};
			r.type = MenuEntryType::button;
			r.entryTag = button->tag;
			strcpy(r.button.response, button->tag.c_str());
			results.emplace_back(std::move(r));
		}
	}

	for (auto& textBoxId : text_boxes) {
		MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
		textBox->update(mousePos, toggled);

		//if we were activated, enter all other text boxes data
		if (textBox->pollActivated()) {
			for (auto& otherTextBoxId : text_boxes) {
				if (otherTextBoxId != textBoxId) {
					MenuTextBoxComponent* otherTextBox = EntitySystem::GetComp<MenuTextBoxComponent>(otherTextBoxId);
					if(otherTextBox->isActive())
						otherTextBox->enterText();
				}
			}
		}

		std::string response{};
		if (textBox->pollEnteredText(response)) {
			MenuResult r{};
			r.type = MenuEntryType::text_box;
			r.entryTag = textBox->tag;
			strcpy(r.text_box.resposne, response.substr(0, 63).c_str());
			results.emplace_back(std::move(r));
		}
	}

	for (auto& gridId : grids) {
		MenuGridComponent* grid = EntitySystem::GetComp<MenuGridComponent>(gridId);
		grid->update(mousePos, toggled);

		if (grid->pollToggled()) {
			MenuResult r{};
			r.type = MenuEntryType::grid;
			r.entryTag = grid->tag;
			strcpy(r.grid.response, grid->selected.c_str());
			results.emplace_back(std::move(r));
		}
	}

	prevButton1 = currButton1;
}

void Menu::input(const std::string& text) {
	for (auto& textBoxId : text_boxes) {
		MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
		if (textBox) {
			if (textBox->isActive()) {
				textBox->input(text);
			}
		}
	}
}

void Menu::backspace() {
	for (auto& textBoxId : text_boxes) {
		MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
		if (textBox->isActive()) {
			textBox->backspace();
		}
	}
}

void Menu::enterText() {
	for (auto& textBoxId : text_boxes) {
		MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
		if (textBox->isActive()) {
			textBox->enterText();
		}
	}
}

EntityId Menu::makeButton(const std::string& entryTag_, const AABB& boundingBox) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<MenuButtonComponent>(1, &id);

	MenuButtonComponent* button = EntitySystem::GetComp<MenuButtonComponent>(id);
	NDC* data = EntitySystem::GetComp<NetworkDataComponent>(id);

	data->set<float>(X, boundingBox.pos.x);
	data->set<float>(Y, boundingBox.pos.y);

	button->boundingBox = boundingBox;
	button->tag = entryTag_;

	buttons.push_back(id);
	return id;
}

EntityId Menu::makeTextBox(const std::string& entryTag_, const AABB& boundingBox) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<MenuTextBoxComponent>(1, &id);

	MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(id);
	NDC* data = EntitySystem::GetComp<NetworkDataComponent>(id);

	data->set<float>(X, boundingBox.pos.x);
	data->set<float>(Y, boundingBox.pos.y);

	textBox->boundingBox = boundingBox;
	textBox->tag = entryTag_;

	text_boxes.push_back(id);
	return id;
}

EntityId Menu::makeGrid(const std::string& entryTag_, const AABB& boundingBox) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<MenuGridComponent>(1, &id);

	MenuGridComponent* grid = EntitySystem::GetComp<MenuGridComponent>(id);
	NDC* data = EntitySystem::GetComp<NetworkDataComponent>(id);

	data->set<float>(X, boundingBox.pos.x);
	data->set<float>(Y, boundingBox.pos.y);

	grid->boundingBox = boundingBox;
	grid->tag = entryTag_;

	grids.push_back(id);
	return id;
}

const std::vector<EntityId>& Menu::getButtons() const {
	return buttons;
}

const std::vector<EntityId>& Menu::getTextBoxes() const {
	return text_boxes;
}

const std::vector<EntityId>& Menu::getGrids() const {
	return grids;
}

EntityId Menu::getMenuEntry(const std::string& entryTag_) {
	for (auto& buttonId : buttons) {
		MenuButtonComponent* button = EntitySystem::GetComp<MenuButtonComponent>(buttonId);
		if (button->tag == entryTag_)
			return buttonId;
	}
	for (auto& textBoxId : text_boxes) {
		MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
		if (textBox->tag == entryTag_)
			return textBoxId;
	}
	for (auto& gridId : grids) {
		MenuGridComponent* grid = EntitySystem::GetComp<MenuGridComponent>(gridId);
		if (grid->tag == entryTag_)
			return gridId;
	}

	return 0;
}

void Menu::clear() {
	buttons.clear();
	text_boxes.clear();
	grids.clear();
}
