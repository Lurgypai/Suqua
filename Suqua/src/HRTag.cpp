#include "HRTag.h"

HRTag::HRTag(unsigned long long idNum_, CharBuffer<TAG_LENGTH> idTag_) :
	idNum{idNum_},
	idTag{idTag_}
{}

HRTag::HRTag(const HRTag& other) :
	idNum{other.idNum},
	idTag{other.idTag}
{}

HRTag& HRTag::operator=(const HRTag& other) {
	idTag = other.idTag;
	idNum = other.idNum;

	return *this;
}

bool HRTag::operator==(const HRTag& other) {
	return idTag == other.idTag && idNum == other.idNum;
}

bool HRTag::operator!=(const HRTag& other){
	return !(*this == other);
}

bool HRTag::sameNum(const HRTag& other) {
	return idNum == other.idNum;
}

bool HRTag::sameTag(const HRTag& other) {
	return idTag == other.idTag;
}
