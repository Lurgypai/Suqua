#include "SDL.h"

#include "Menu.h"
#include "MenuButtonComponent.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "RectDrawable.h"
#include "GLRenderer.h"
#include "DebugIO.h"

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

	prevButton1 = currButton1;
}

EntityId Menu::makeButton(const std::string& entryTag_, const AABB& boundingBox) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<MenuButtonComponent>(1, &id);

	MenuButtonComponent* button = EntitySystem::GetComp<MenuButtonComponent>(id);
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);

	pos->pos = boundingBox.pos;

	button->boundingBox = boundingBox;
	button->tag = entryTag_;

	buttons.push_back(id);
	return id;
}

EntityId Menu::makeTextBox(const std::string& entryTag_, const AABB& boundingBox)
{
	return EntityId();
}

const std::vector<EntityId>& Menu::getButtons() const {
	return buttons;
}