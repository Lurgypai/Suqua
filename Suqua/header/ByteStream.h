#pragma once
#include <vector>
#include <cstring>
#include <string>

#include "UUID.h"
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
	bool peek(T& t) const;

	bool operator==(const ByteStream& other);
	bool operator!=(const ByteStream& other);

	bool sameData(const ByteStream& other);

    void allocateData(size_t len);
	void putData(void* data, size_t len);
    std::string getData();

	void setPos(size_t pos_);
    void movePos(size_t offset);
    size_t getPos();
    size_t size();

	bool hasMoreData();
	const Byte* data() const;
	void clear();
	size_t size() const;
private:
	size_t pos;
	ByteArray _data;
};

template<typename T>
inline ByteStream& ByteStream::operator<<(const T& t) {
	size_t end = _data.size();
    size_t newEnd = pos + sizeof(T);
    if(newEnd > end) _data.resize(end + (newEnd - end));

	T cpy = s_hton(t);
	std::memcpy(_data.data() + pos, &cpy, sizeof(T));
    pos += sizeof(T);

	return *this;
}

template<>
inline ByteStream& ByteStream::operator<< <std::string>(const std::string& t) {
	size_t end = _data.size();
    size_t newEnd = pos + sizeof(size_t) + t.size();

    if(newEnd > end) _data.resize(end + (newEnd - end));

	size_t strSize = s_hton(t.size());
	std::memcpy(_data.data() + pos, &strSize, sizeof(size_t));
	std::memcpy(_data.data() + pos + sizeof(size_t), t.data(), t.size());

    pos += sizeof(size_t) + t.size();

	return *this;
}

template<>
inline ByteStream& ByteStream::operator<< <Vec2f>(const Vec2f& t) {
	size_t end = _data.size();
    size_t newEnd = pos + sizeof(Vec2f);
    if(newEnd > end) _data.resize(end + (newEnd - end));

	auto xCpy = s_hton(t.x);
	auto yCpy = s_hton(t.y);
	std::memcpy(_data.data() + pos, &xCpy, sizeof(xCpy));
	std::memcpy(_data.data() + pos + sizeof(xCpy), &yCpy, sizeof(yCpy));
    pos += sizeof(Vec2f);

    return *this;
}

template<>
inline ByteStream& ByteStream::operator<< <Suqua::UUID>(const Suqua::UUID& t) {
	size_t end = _data.size();
    size_t newEnd = pos + sizeof(Suqua::UUID);
    if(newEnd > end) _data.resize(end + (newEnd - end));

	auto xCpy = s_hton(t.data_[0]);
	auto yCpy = s_hton(t.data_[1]);
	std::memcpy(_data.data() + pos, &xCpy, sizeof(xCpy));
	std::memcpy(_data.data() + pos + sizeof(xCpy), &yCpy, sizeof(yCpy));
    pos += sizeof(Suqua::UUID);

    return *this;
}

template<typename T>
inline bool ByteStream::operator>>(T& t) {
    if(pos + sizeof(t) > _data.size()) return false;

    std::memcpy(&t, _data.data() + pos, sizeof(t));
    t = s_ntoh(t);
    pos += sizeof(t);
    return true;
}

template<>
inline bool ByteStream::operator>> <std::string>(std::string& s) {
    if (pos + sizeof(size_t) > _data.size()) return false;

    size_t size;
    std::memcpy(&size, _data.data() + pos, sizeof(size_t));
    size = s_ntoh(size);

    if(pos + sizeof(size_t) + size > _data.size()) return false;

    s.resize(size);
    std::memcpy(s.data(), _data.data() + pos + sizeof(size_t), size);

    pos += size + sizeof(size_t);
    return true;
}

template<>
inline bool ByteStream::operator >> <Vec2f>(Vec2f& v) {
	if (pos + sizeof(v) > _data.size()) return false;

	std::memcpy(&v.x, _data.data() + pos, sizeof(v.x));
    pos += sizeof(v.x);
	std::memcpy(&v.y, _data.data() + pos, sizeof(v.y));
    pos += sizeof(v.y);

	v.x = s_ntoh(v.x);
	v.y = s_ntoh(v.y);

	return true;
}

template<>
inline bool ByteStream::operator >> <Suqua::UUID>(Suqua::UUID& v) {
	if (pos + sizeof(v) > _data.size()) return false;

	std::memcpy(&v.data_[0], _data.data() + pos, sizeof(v.data_[0]));
    pos += sizeof(v.data_[0]);
	std::memcpy(&v.data_[1], _data.data() + pos, sizeof(v.data_[1]));
    pos += sizeof(v.data_[1]);

	v.data_[0] = s_ntoh(v.data_[0]);
	v.data_[1] = s_ntoh(v.data_[1]);

	return true;
}

template<typename T>
inline bool ByteStream::peek(T& t) const {
	if (pos + sizeof(t) <= _data.size()) {
		std::memcpy(&t, _data.data() + pos, sizeof(t));
		t = s_ntoh(t);
		return true;
	}
	else {
		return false;
	}
}

template<>
inline bool ByteStream::peek<std::string>(std::string& s) const {
	if (pos + sizeof(size_t) <= _data.size()) {
		size_t size;
		std::memcpy(&size, _data.data() + pos, sizeof(size_t));
		size = s_ntoh(size);
		if (pos + sizeof(size_t) + size <= _data.size()) {
			s.resize(size);
			std::memcpy(s.data(), _data.data() + pos, size);
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
