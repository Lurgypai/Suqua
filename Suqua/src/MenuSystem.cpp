#include "MenuSystem.h"

//stop rebuilding
MenuSystem::MenuSystem() :
	menus{},
	currMenuId{1}
{}

MenuId MenuSystem::makeMenu() {
	MenuId id = currMenuId++;
	menus[id] = Menu{id};
	return id;
}

Menu& MenuSystem::getMenu(MenuId id_) {
	auto pair = menus.find(id_);
	if (pair != menus.end()) {
		return pair->second;
	}
	else {
		throw std::exception{};
	}
}

void MenuSystem::inputAllMenus(const std::string& text) {
	for (auto& menuPair : menus) {
		menuPair.second.input(text);
	}
}

void MenuSystem::backspaceAllMenus() {
	for (auto& menuPair : menus) {
		menuPair.second.backspace();
	}
}

void MenuSystem::enterTextAllMenus() {
	for (auto& menuPair : menus) {
		menuPair.second.enterText();
	}
}
