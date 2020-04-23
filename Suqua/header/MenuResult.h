#pragma once
#include <string>

//both the type of a menu entry used to describe it, and the type returned when an entry is pushed
enum class MenuEntryType {
	none,
	button,
	text_box
};

struct MenuResult {

	MenuEntryType type;
	std::string entryTag;
	union {
		struct {
			char response[8];
		} button;
		struct {
			char resposne[64];
		} text_box;
	};
};