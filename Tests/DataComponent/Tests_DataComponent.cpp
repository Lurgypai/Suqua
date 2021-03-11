#include <iostream>
#include "DataComponent.h"
#include <iostream>
#include "TestUtil.h"

/*
*	DATACOMPONENT FUNCTIONS:
	void serializeForNetwork(MarkedStream& stream);
	void serializeForNetwork(MarkedStream& stream, std::map<DataId, Data> prevData);
	void unserialize(const MarkedStream& stream);

	DATA
		template<typename T>
		T& get();

		template<typename T>
		void set(const T& t);

		template<typename T>
		void set(T&& t);

		ByteArray serialize();
*/

int main() {
	DataComponent dataComp{};
	dataComp.set<char>(0, 'a');
	dataComp.set<uint32_t>(1, 1);
	dataComp.set<int32_t>(2, 2);
	dataComp.set<uint64_t>(3, 3);
	dataComp.set<int64_t>(4, 4);
	dataComp.set<double>(5, 3.5);
	dataComp.set<std::string>(6, "foo");

	char zero = dataComp.get<char>(0);
	uint32_t one = dataComp.get<uint32_t>(1);
	int32_t two = dataComp.get<int32_t>(2);
	uint64_t three = dataComp.get<uint64_t>(3);
	int64_t four = dataComp.get<int64_t>(4);
	double five = dataComp.get<double>(5);
	std::string six = dataComp.get<std::string>(6);

	std::cout << "Testing...\n";

	std::cout << "Testing value reading...\n";

	TestUtil::expect(zero, 'a');
	uint32_t v = 1;
	TestUtil::expect(one, v);
	int32_t v2 = 2;
	TestUtil::expect(two, v2);
	uint64_t v3 = 3;
	TestUtil::expect(three, v3);
	int64_t v4 = 4;
	TestUtil::expect(four, v4);
	TestUtil::expect(five, 3.5);
	TestUtil::expect(six, std::string{ "foo" });
	std::cout << "Value read complete.\n";

	std::cout << "Testing throw...\n";
	try {
		dataComp.get<double>(8);
	}
	catch (std::exception& e) {
		std::cout << "Exception Caught.\n";
	}

	ByteStream stream;
	dataComp.serializeForNetwork(stream);


	dataComp.set<char>(0, 'b');
	dataComp.set<uint32_t>(1, 10);
	dataComp.set<int32_t>(2, 20);
	dataComp.set<uint64_t>(3, 30);
	dataComp.set<int64_t>(4, 40);
	dataComp.set<double>(5, 7.0);
	dataComp.set<std::string>(6, "baz");

	dataComp.serializeForNetwork(stream);

	std::cout << "Testing first read from stream...\n";

	dataComp.unserialize(stream);

	zero = dataComp.get<char>(0);
	one = dataComp.get<uint32_t>(1);
	two = dataComp.get<int32_t>(2);
	three = dataComp.get<uint64_t>(3);
	four = dataComp.get<int64_t>(4);
	five = dataComp.get<double>(5);
	six = dataComp.get<std::string>(6);

	TestUtil::expect(zero, 'a');
	v = 1;
	TestUtil::expect(one, v);
	v2 = 2;
	TestUtil::expect(two, v2);
	v3 = 3;
	TestUtil::expect(three, v3);
	v4 = 4;
	TestUtil::expect(four, v4);
	TestUtil::expect(five, 3.5);
	TestUtil::expect(six, std::string{ "foo" });
	std::cout << "First read complete.\n";


	std::cout << "Testing second read from stream...\n";

	dataComp.unserialize(stream);

	zero = dataComp.get<char>(0);
	one = dataComp.get<uint32_t>(1);
	two = dataComp.get<int32_t>(2);
	three = dataComp.get<uint64_t>(3);
	four = dataComp.get<int64_t>(4);
	five = dataComp.get<double>(5);
	six = dataComp.get<std::string>(6);

	TestUtil::expect(zero, 'b');
	v = 10;
	TestUtil::expect(one, v);
	v2 = 20;
	TestUtil::expect(two, v2);
	v3 = 30;
	TestUtil::expect(three, v3);
	v4 = 40;
	TestUtil::expect(four, v4);
	TestUtil::expect(five, 7.0);
	TestUtil::expect(six, std::string{ "baz" });
	std::cout << "Second read complete.\n";

	stream.clear();

	auto prevState = dataComp.data();

	dataComp.set<uint32_t>(1, 100);
	dataComp.set<uint64_t>(3, 500);


	dataComp.serializeForNetwork(stream, prevState);

	dataComp.set<uint32_t>(1, 5);
	dataComp.set<uint64_t>(3, 5);

	std::cout << "Reading from partial stream...\n";
	dataComp.unserialize(stream);

	one = dataComp.get<uint32_t>(1);
	two = dataComp.get<int32_t>(2);
	three = dataComp.get<uint64_t>(3);

	v = 100;
	TestUtil::expect(one, v);
	v2 = 20;
	TestUtil::expect(two, v2);
	v3 = 500;
	TestUtil::expect(three, v3);
	std::cout << "Testing complete.\n";

	return 0;
}