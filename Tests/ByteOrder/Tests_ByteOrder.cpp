#include <iostream>
#include "ByteOrder.h"
#include <iostream>
#include "TestUtil.h"

int main() {
	uint32_t x = 5;
	uint64_t y = 10;
	int32_t a = 7;
	int64_t b = 8;
	float f = 5.0f;
	double d = 6.0;

	/*
	uint32_t x2 = s_htonl(x);
	int32_t a2 = s_htonl(a);

	uint64_t y2 = s_htonll(y);
	int64_t b2 = s_htonll(b);

	x2 = s_ntohl(x2);
	a2 = s_ntohl(a2);

	y2 = s_htonl(y2);
	b2 = s_htonl(b2);

	uint32_t fs = s_htonf(f);
	uint64_t ds = s_htond(d);

	float f2 = s_ntohf(fs);
	double d2 = s_ntohd(ds);

	std::cout << "Comparing...\n";

	TestUtil::expect(x, x2);
	TestUtil::expect(a, a2);

	TestUtil::expect(y, y2);
	TestUtil::expect(b, b2);

	TestUtil::expect(f, f2);
	TestUtil::expect(d, d2);

	std::cout << "Complete.\n";
	*/

	auto x2 = s_hton(x);
	auto a2 = s_hton(a);

	auto y2 = s_hton(y);
	auto b2 = s_hton(b);

	x2 = s_ntoh(x2);
	a2 = s_ntoh(a2);

	y2 = s_ntoh(y2);
	b2 = s_ntoh(b2);

	auto fs = s_hton(f);
	auto ds = s_hton(d);

	auto f2 = s_ntoh(fs);
	auto d2 = s_ntoh(ds);

	std::cout << "Using templated version...\n";

	TestUtil::expect(x, x2);
	TestUtil::expect(a, a2);

	TestUtil::expect(y, y2);
	TestUtil::expect(b, b2);

	TestUtil::expect(f, f2);
	TestUtil::expect(d, d2);

	std::cout << "Complete.\n";

	return 0;
}
