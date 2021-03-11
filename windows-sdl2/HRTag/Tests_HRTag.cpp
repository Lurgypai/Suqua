#include <iostream>
#include "HRTag.h"
#include <iostream>
#include "TestUtil.h"

void printTag(const HRTag& tag) {
	std::cout << "Number: " << tag.idNum << ", Tag: " << tag.idTag << '\n';
}

int main() {
	HRTag tag1;
	
	printTag(tag1);

	HRTag tag2{ 0, CharBuffer<16>{"test"} };

	printTag(tag2);

	tag1 = tag2;

	printTag(tag1);

	std::cout << '\n';

	std::cout << "Running tests...\n";
	TestUtil::expect(tag1 == tag1, true);
	TestUtil::expect(tag1 != tag1, false);
	TestUtil::expect(tag1 == tag2, true);
	TestUtil::expect(tag1 != tag2, false);
	TestUtil::expect(tag1.sameNum(tag2), true);
	TestUtil::expect(tag1.sameTag(tag2), true);

	tag1 = HRTag{ 1, CharBuffer<16>{"hey"} };

	TestUtil::expect(tag1 == tag2, false);
	TestUtil::expect(tag1 != tag2, true);
	TestUtil::expect(tag1.sameNum(tag2), false);
	TestUtil::expect(tag1.sameTag(tag2), false);

	tag2 = HRTag{ 0, CharBuffer<16>{"hey"} };

	TestUtil::expect(tag1 == tag2, false);
	TestUtil::expect(tag1 != tag2, true);
	TestUtil::expect(tag1.sameNum(tag2), false);
	TestUtil::expect(tag1.sameTag(tag2), true);
	std::cout << "Tests complete.\n\n";

	return 0;
}