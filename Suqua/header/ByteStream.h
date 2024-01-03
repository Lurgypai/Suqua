#pragma once
#include <vector>
#include <cstring>
#include <string>
#include "Vec2.h"
#include "ByteOrder.h"

using Byte = char;
using ByteArray = std::vector<Byte>;

//Store bytes in network byte order
class ByteStream {
public:
	ByteStream();

	template<typename T>
	ByteStream& operator<<(const T& t);

	template<typename T>
	bool operator>>(T& t);

	template<typename T>
	bool peek(T& t);

	bool operator==(const ByteStream& other);
	bool operator!=(const ByteStream& other);

	bool sameData(const ByteStream& other);

	void putData(void* data, size_t len);
    std::string getData();

	void setReadPos(size_t readPos_);

	bool hasMoreData();
	const Byte* data() const;
	void clear();
	size_t size() const;
private:
	size_t readPos;
	ByteArray _data;
};

template<typename T>
inline ByteStream& ByteStream::operator<<(const T& t) {
	size_t end = _data.size();
	_data.resize(end + sizeof(T));
	T cpy = s_hton(t);
	std::memcpy(_data.data() + end, &cpy, sizeof(T));
	return *this;
}

template<>
inline ByteStream& ByteStream::operator<< <std::string>(const std::string& t) {
	size_t end = _data.size();
	size_t strSize = s_hton(t.size());
	_data.resize(end + sizeof(size_t));
	std::memcpy(_data.data() + end, &strSize, sizeof(size_t));
	end = _data.size();
	_data.resize(end + strSize);
	std::memcpy(_data.data() + end, t.data(), strSize);

	return *this;
}

template<>
inline ByteStream& ByteStream::operator<< <Vec2f>(const Vec2f& t) {
	size_t end = _data.size();
	_data.resize(end + sizeof(t));
	auto xCpy = s_hton(t.x);
	auto yCpy = s_hton(t.y);
	std::memcpy(_data.data() + end, &xCpy, sizeof(xCpy));
	std::memcpy(_data.data() + end, &yCpy, sizeof(yCpy));

    return *this;
}

template<typename T>
inline bool ByteStream::operator>>(T& t) {
	if (readPos + sizeof(t) <= _data.size()) {
		std::memcpy(&t, _data.data() + readPos, sizeof(t));
		t = s_ntoh(t);
		readPos += sizeof(t);
		return true;
	}
	else {
		return false;
	}
}

template<>
inline bool ByteStream::operator>> <std::string>(std::string& s) {
	if (readPos + sizeof(size_t) <= _data.size()) {
		size_t size;
		std::memcpy(&size, _data.data() + readPos, sizeof(size_t));
		size = s_ntoh(size);
		if (readPos + sizeof(size_t) + size <= _data.size()) {
			readPos += sizeof(size_t);
			s.resize(size);
			std::memcpy(s.data(), _data.data() + readPos, size);
			readPos += size;
            return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

template<>
inline bool ByteStream::operator >> <Vec2f>(Vec2f& v) {
	if (readPos + sizeof(v) > _data.size()) return false;

	std::memcpy(&v.x, _data.data() + readPos, sizeof(v.x));
	std::memcpy(&v.y, _data.data() + readPos, sizeof(v.y));

	v.x = s_ntoh(v.x);
	v.y = s_ntoh(v.y);

	return true;
}

template<typename T>
inline bool ByteStream::peek(T& t) {
	if (readPos + sizeof(t) <= _data.size()) {
		std::memcpy(&t, _data.data() + readPos, sizeof(t));
		t = s_ntoh(t);
		return true;
	}
	else {
		return false;
	}
}

template<>
inline bool ByteStream::peek<std::string>(std::string& s) {
	if (readPos + sizeof(size_t) <= _data.size()) {
		size_t size;
		std::memcpy(&size, _data.data() + readPos, sizeof(size_t));
		size = s_ntoh(size);
		if (readPos + sizeof(size_t) + size <= _data.size()) {
			s.resize(size);
			std::memcpy(s.data(), _data.data() + readPos, size);
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}
