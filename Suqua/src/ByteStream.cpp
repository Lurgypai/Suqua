#include "..\header\ByteStream.h"

ByteStream::ByteStream() :
	readPos{0}
{}

void ByteStream::setReadPos(size_t readPos_) {
	readPos = readPos_;
}

void ByteStream::writeData(void* data, size_t len) {
	auto start = _data.size();
	_data.resize(start + len);
	std::memcpy(_data.data() + start, data, len);
}

bool ByteStream::hasMoreData() {
	return readPos != _data.size();
}

const Byte* ByteStream::data() const {
	return _data.data();
}

void ByteStream::clear() {
	readPos = 0;
	_data.clear();
}

size_t ByteStream::size() const {
	return _data.size();
}