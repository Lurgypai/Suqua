#pragma once
#include <cstdint>
#include <vector>


class DynamicBitset {
public:
	DynamicBitset();
	void resize(std::size_t newSize);
	void zero();
	bool operator[](std::size_t pos) const;
	std::size_t size() const;
	void set(std::size_t bitPos, bool state);
private:
	using Int = uint64_t;

	std::size_t size_;
	std::vector<Int> bits;
};
