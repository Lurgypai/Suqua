#pragma once
#include <string>

//both the type of a menu entry used to describe it, and the type returned when an entry is pushed
enum class MenuEntryType {
	none,
	button,
	text_box
};

struct MenuResult {
	MenuResult();
	MenuResult(const MenuResult& other);
	MenuResult& operator=(const MenuResult& other);
	~MenuResult();

	MenuEntryType type;
	std::string entryTag;
	union {
		struct {
			std::string response;
		} button;
		struct {
			std::string response;
		} text_box;
	};
};