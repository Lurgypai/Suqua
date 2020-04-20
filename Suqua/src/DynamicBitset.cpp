#include "DynamicBitset.h"
#include <algorithm>
#include <exception>

DynamicBitset::DynamicBitset() :
	bits{},
	size_{0}
{}

void DynamicBitset::resize(size_t newBitSize) {
	//8 bits in a byte
	constexpr auto INT_BITS = sizeof(Int) * 8;
	size_t allocatedBits = bits.size() * INT_BITS;
	if (allocatedBits >= newBitSize) {
		size_ = newBitSize;
	}
	else {
		//integer division ceil()
		bits.resize((newBitSize + INT_BITS - 1) / INT_BITS);
		size_ = newBitSize;
	}
}

void DynamicBitset::zero() {
	std::fill(bits.begin(), bits.end(), 0);
}

bool DynamicBitset::operator[](size_t bitPos) const {
	constexpr auto INT_BITS = sizeof(Int) * 8;
	size_t intPos = bitPos / INT_BITS;
	size_t localBitPos = bitPos - (intPos * INT_BITS);
	Int bit = 1 << localBitPos;
	return bits[intPos] & bit;
}

size_t DynamicBitset::size() const {
	return size_;
}

void DynamicBitset::set(size_t bitPos, bool state) {
	constexpr auto INT_BITS = sizeof(Int) * 8;
	size_t intPos = bitPos / INT_BITS;
	size_t localBitPos = bitPos - (intPos * INT_BITS);
	Int bit = 1 << localBitPos;
	if (state)
		bits[intPos] |= bit;
	else
		bits[intPos] &= ~bit;
}
