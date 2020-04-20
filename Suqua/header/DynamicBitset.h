#pragma once
#include <cstdint>
#include <vector>


class DynamicBitset {
public:
	DynamicBitset();
	void resize(size_t newSize);
	void zero();
	bool operator[](size_t pos) const;
	size_t size() const;
	void set(size_t bitPos, bool state);
private:
	using Int = uint64_t;

	size_t size_;
	std::vector<Int> bits;
};