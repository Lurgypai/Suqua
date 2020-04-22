#include "MenuSystem.h"

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