#pragma once
#include <vector>
#include <cstring>
#include <iterator>

using ByteArray = std::vector<char>;

class MarkedStream {
public:
	MarkedStream();
	MarkedStream(const MarkedStream& other);
	MarkedStream(void* data, size_t size);
	void addEmptyField();
	void clear();
	bool hasMoreData();
	size_t size();
	size_t getReadPos() const;
	char* data();

	template<typename T>
	void operator<<(const T& inData);

	template<typename T>
	bool operator>>(T& outData);

private:
	ByteArray data_;
	size_t readPos;
};

template<typename T>
inline void MarkedStream::operator<<(const T& inData) {
	data_.emplace_back(1);
	size_t end = data_.size();
	data_.resize(end + sizeof(T));
	std::memcpy(&data_[end], &inData, sizeof(T));
}

template<>
inline void MarkedStream::operator<< <ByteArray>(const ByteArray& inData) {
	data_.emplace_back(1);
	size_t end = data_.size();
	data_.resize(end + sizeof(size_t));
	size_t s = inData.size();
	std::memcpy(data_.data() + end, &s, sizeof(s));
	end = data_.size();
	data_.reserve(end + inData.size());
	std::copy(inData.begin(), inData.end(), std::back_inserter(data_));
}

template<typename T>
inline bool MarkedStream::operator>>(T& outData) {
	bool ret = data_[readPos];
	++readPos;

	if (!ret)
		return ret;

	std::memcpy(&outData, &data_[readPos], sizeof(outData));
	readPos += sizeof(T);
	return ret;
}

template<>
inline bool MarkedStream::operator>> <ByteArray>(ByteArray& outData) {
	bool ret = data_[readPos];
	++readPos;

	if (!ret)
		return ret;

	size_t size;
	std::memcpy(&size, data_.data() + readPos, sizeof(size));
	outData.resize(size);
	readPos += sizeof(size);
	std::memcpy(outData.data(), data_.data() + readPos, size);
	readPos += size;
	return ret;
}