#include "MarkedStream.h"

MarkedStream::MarkedStream() :
	data_{},
	readPos{}
{}

MarkedStream::MarkedStream(const MarkedStream& other) :
	data_{ other.data_ },
	readPos{other.readPos}
{}

MarkedStream::MarkedStream(void* data, size_t size) :
	data_{},
	readPos{}
{
	data_.resize(size);
	std::memcpy(data_.data(), data, size);
}

void MarkedStream::addEmptyField() {
	data_.emplace_back(0);
}

void MarkedStream::clear() {
	data_.clear();
	readPos = 0;
}

bool MarkedStream::hasMoreData() {
	return readPos != data_.size();
}

size_t MarkedStream::size() {
	return data_.size();
}

size_t MarkedStream::getReadPos() const {
	return readPos;
}

char* MarkedStream::data() {
	return data_.data();
}