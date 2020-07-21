#pragma once
#include <vector>
#include <cstring>

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
	std::vector<char> data_;
	size_t readPos;
};

template<typename T>
inline void MarkedStream::operator<<(const T& inData) {
	data_.emplace_back(1);
	size_t end = data_.size();
	data_.resize(end + sizeof(T));
	std::memcpy(&data_[end], &inData, sizeof(T));
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
