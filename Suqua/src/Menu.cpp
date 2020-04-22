#include "SDL.h"

#include "Menu.h"
#include "MenuButtonComponent.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "RectDrawable.h"
#include "GLRenderer.h"

Menu::Menu()
{}

Menu::Menu(MenuId id_) :
	id{id_},
	results{},
	buttons{},
	text_boxes{}
{}

void Menu::addMenuEntry(MenuEntryType type, const std::string& entryTag_, const AABB& boundingBox) {
	switch (type) {
	case MenuEntryType::button:
		makeButton(entryTag_, boundingBox);
	case MenuEntryType::text_box:
		makeTextBox(entryTag_, boundingBox);
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

	for (auto& buttonId : buttons) {
		MenuButtonComponent* button = EntitySystem::GetComp<MenuButtonComponent>(buttonId);
		button->update(mousePos);
		if (button->pollToggled()) {
			MenuResult r;
			r.type = MenuEntryType::button;
			r.entryTag = button->tag;
			r.button.response = button->tag;
			results.emplace_back(std::move(r));
		}
	}
}

EntityId Menu::makeButton(const std::string& entryTag_, const AABB& boundingBox) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<MenuButtonComponent>(1, &id);
	EntitySystem::MakeComps<RenderComponent>(1, &id);

	MenuButtonComponent* button = EntitySystem::GetComp<MenuButtonComponent>(id);
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);

	pos->pos = boundingBox.pos;

	button->boundingBox = boundingBox;
	button->tag = entryTag_;
	
	render->loadDrawable<RectDrawable>();
	auto* drawable = render->getDrawable<RectDrawable>();
	drawable->shape = boundingBox;
	drawable->r = 1;
	drawable->g = 1;
	drawable->b = 1;
	return id;
}

EntityId Menu::makeTextBox(const std::string& entryTag_, const AABB& boundingBox)
{
	return EntityId();
}