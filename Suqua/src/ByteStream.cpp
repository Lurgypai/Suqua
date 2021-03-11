#include "..\header\ByteStream.h"

ByteStream::ByteStream() :
	readPos{0}
{}

void ByteStream::setReadPos(size_t readPos_) {
	readPos = readPos_;
}

bool ByteStream::hasMoreData() {
	return readPos != _data.size();
}

void ByteStream::clear() {
	readPos = 0;
	_data.clear();
}