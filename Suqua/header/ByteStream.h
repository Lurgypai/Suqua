#pragma once
#include <vector>
#include <cstring>
#include <string>

using ByteArray = std::vector<char>;

class ByteStream {
public:
	ByteStream();

	template<typename T>
	ByteStream& operator<<(const T& t);

	template<typename T>
	bool operator>>(T& t);

	void setReadPos(size_t readPos_);

	bool hasMoreData();

	void clear();
private:
	size_t readPos;
	ByteArray _data;
};

template<typename T>
inline ByteStream& ByteStream::operator<<(const T& t) {
	size_t end = _data.size();
	_data.resize(end + sizeof(T));
	std::memcpy(_data.data() + end, &t, sizeof(T));
	return *this;
}

template<>
inline ByteStream& ByteStream::operator<< <std::string>(const std::string& t) {
	size_t end = _data.size();
	size_t strSize = t.size();
	_data.resize(end + sizeof(size_t));
	std::memcpy(_data.data() + end, &strSize, sizeof(size_t));
	end = _data.size();
	_data.resize(end + strSize);
	std::memcpy(_data.data() + end, t.data(), strSize);

	return *this;
}

template<typename T>
inline bool ByteStream::operator>>(T& t) {
	if (readPos + sizeof(t) <= _data.size()) {

		std::memcpy(&t, _data.data() + readPos, sizeof(t));

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
		if (readPos + sizeof(size_t) + size <= _data.size()) {
			readPos += sizeof(size_t);
			s.resize(size);
			std::memcpy(s.data(), _data.data() + readPos, size);
			readPos += size;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}
