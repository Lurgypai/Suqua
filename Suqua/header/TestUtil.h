#pragma once
#include <iostream>

namespace TestUtil {
	template<typename T>
	void expect(const T& first, const T& second) {
		if (first != second) {
			std::cout << "Expected: " << second << ", Actual: " << first << '\n';
		}
	}
}
