#include <iostream>
#include "CharBuffer.h"

int main() {

	CharBuffer<10> c;
	c = "aaaaaaaaaa";
	c = "test";

	for (auto i = 0; i != c.size(); ++i) {
		std::cout << c[i] << '\n';
	}

	try {
		CharBuffer<5> a{ "012345" };
	}
	catch (std::exception e) {
		std::cout << "Threw.\n";
	}


	return 0;
}