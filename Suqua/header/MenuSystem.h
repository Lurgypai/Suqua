#pragma once
#include <cstdint>
#include <unordered_map>

#include "MenuId.h"
#include "Menu.h"

class MenuSystem {
public:
	MenuSystem();

	MenuId makeMenu();
	Menu& getMenu(MenuId id_);

	void inputAllMenus(const std::string& text);
	void backspaceAllMenus();
	void enterTextAllMenus();

private:
	std::unordered_map<MenuId, Menu> menus;
	MenuId currMenuId;
};
