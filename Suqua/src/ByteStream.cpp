#include "ByteStream.h"
#include <cstring>

ByteStream::ByteStream() :
	pos{0}
{}

bool ByteStream::operator==(const ByteStream& other) {
	return _data == other._data && pos == other.pos;
}

bool ByteStream::operator!=(const ByteStream& other) {
	return !(*this == other);
}

bool ByteStream::sameData(const ByteStream& other) {
	return _data == other._data;
}

void ByteStream::setPos(size_t pos_) {
	pos = pos_;
}

void ByteStream::movePos(size_t offset) {
    pos += offset;
}

size_t ByteStream::getPos() {
    return pos;
}

size_t ByteStream::size() {
    return _data.size();
}

void ByteStream::allocateData(size_t len) {
	auto start = _data.size();
	_data.resize(start + len);
    pos += len;
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
	return pos != _data.size();
}

const Byte* ByteStream::data() const {
	return _data.data();
}

void ByteStream::clear() {
	pos = 0;
	_data.clear();
}

size_t ByteStream::size() const {
	return _data.size();
}
