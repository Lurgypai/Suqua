#include "MenuResult.h"

MenuResult::MenuResult()
{}

MenuResult::~MenuResult() {
	switch (type) {
	case MenuEntryType::button:
		button.response.~basic_string();
		break;
	case MenuEntryType::text_box:
		text_box.response.~basic_string();
		break;
	}
}

MenuResult::MenuResult(const MenuResult& other):
	type{other.type},
	entryTag{other.entryTag}
{
	switch (other.type) {
	case MenuEntryType::button:
		button = other.button;
		break;
	case MenuEntryType::text_box:
		text_box = other.text_box;
		break;
	}
}

MenuResult& MenuResult::operator=(const MenuResult& other) {
	type = other.type;
	entryTag = other.entryTag;

	switch (other.type) {
	case MenuEntryType::button:
		button = other.button;
		break;
	case MenuEntryType::text_box:
		text_box = other.text_box;
		break;
	}

	return *this;
}