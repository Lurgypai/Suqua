#include <iostream>
#include "ByteStream.h"
#include <iostream>
#include "TestUtil.h"


int main() {

	std::cout << "Testing...\n";
	ByteStream stream;
	int x = 5;
	float y = 3.5f;
	bool b = true;
	std::string s = "foo";

	stream << x << y << b << s;

	int x2;
	float y2;
	bool b2;
	std::string s2;

	int x3;
	stream.peek(x3);

	stream >> x2;
	stream >> y2;
	stream >> b2;
	stream >> s2;

	TestUtil::expect(x2, x);
	TestUtil::expect(x2, x3);
	TestUtil::expect(y2, y);
	TestUtil::expect(b2, b);
	TestUtil::expect(s2, s);
	TestUtil::expect(stream >> x2, false);
	TestUtil::expect(x2, x);

	std::cout << "Testing complete.\n";

	return 0;
}