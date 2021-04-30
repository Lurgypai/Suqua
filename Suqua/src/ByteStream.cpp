#include "ByteStream.h"
#include <cstring>

ByteStream::ByteStream() :
	readPos{0}
{}

bool ByteStream::operator==(const ByteStream& other) {
	return _data == other._data && readPos == other.readPos;
}

bool ByteStream::operator!=(const ByteStream& other) {
	return !(*this == other);
}

bool ByteStream::sameData(const ByteStream& other) {
	return _data == other._data;
}

void ByteStream::setReadPos(size_t readPos_) {
	readPos = readPos_;
}

void ByteStream::putData(void* data, size_t len) {
	auto start = _data.size();
	_data.resize(start + len);
	std::memcpy(_data.data() + start, data, len);
}

std::string ByteStream::getData() {
    if(_data.empty())
        return "";
    std::string out;
    out.resize(_data.size());
    std::memcpy(out.data(), _data.data(), _data.size());
    return out;
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
