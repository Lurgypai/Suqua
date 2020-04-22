#pragma once
#include <cstdint>
#include <unordered_map>

#include "MenuId.h"
#include "Menu.h"

class Menu;
enum class MenuEntryType;

class MenuSystem {
public:
	MenuSystem();

	MenuId makeMenu();
	Menu& getMenu(MenuId id_);
private:
	std::unordered_map<MenuId, Menu> menus;
	MenuId currMenuId;
};