#pragma once
#include "CharBuffer.h"

/*
	Human Readable Tag
	Fixed width tag for tagging sections
	POD so that it can be easily copied directly

	its really just an int string pair, so that lookup can be done using either
*/

using TagIdNum = unsigned long long;

class HRTag {
public:
	static constexpr size_t TAG_LENGTH = 16;

	HRTag() = default;
	HRTag(unsigned long long idNum_, CharBuffer<TAG_LENGTH> idTag_);
	HRTag(const HRTag& other);

	HRTag& operator=(const HRTag& other);

	bool operator==(const HRTag& other);
	bool operator!=(const HRTag& other);

	bool sameNum(const HRTag& other);
	bool sameTag(const HRTag& other);

	TagIdNum idNum;
	CharBuffer<TAG_LENGTH> idTag;
};