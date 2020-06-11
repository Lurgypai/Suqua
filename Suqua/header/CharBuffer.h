#pragma once
#include <cstring>
#include <ostream>

template<size_t Size>
class CharBuffer {
public:
	CharBuffer();
	CharBuffer(const char* data_);
	CharBuffer(const CharBuffer& other);
	CharBuffer(const std::string& s);
	CharBuffer& operator=(const CharBuffer& other);
	CharBuffer& operator=(const char* other);
	CharBuffer& operator=(const std::string& s);
	bool operator==(const CharBuffer& other) const;
	bool operator!=(const CharBuffer& other) const;
	operator std::string() const;

	const char* data() const;
	const size_t size();

	template<size_t size_>
	friend std::ostream& operator<<(std::ostream& stream, const CharBuffer<size_>& buffer);
private:
	char _data[Size];
};

template<size_t Size>
inline CharBuffer<Size>::CharBuffer() :
	_data{}
{}


template<size_t Size>
inline CharBuffer<Size>::CharBuffer(const char* data_) :
	_data{}
{
	strcpy(_data, data_);
}

template<size_t Size>
inline CharBuffer<Size>::CharBuffer(const CharBuffer& other) :
	_data{}
{
	strcpy(_data, other._data);
}

template<size_t Size>
inline CharBuffer<Size>::CharBuffer(const std::string& s) :
	_data{}
{
	if (s.size() <= Size)
		strcpy(_data, s.c_str());
	else
		throw std::exception{};
}

template<size_t Size>
inline CharBuffer<Size>& CharBuffer<Size>::operator=(const CharBuffer& other)
{
	strcpy(_data, other._data);
	return *this;
}

template<size_t Size>
inline CharBuffer<Size>& CharBuffer<Size>::operator=(const char* other)
{
	strcpy(_data, other);
	return *this;
}

template<size_t Size>
inline CharBuffer<Size>& CharBuffer<Size>::operator=(const std::string& s)
{
		if (s.size() <= Size)
		strcpy(_data, s.c_str());
	else
		throw std::exception{};
	return *this;
}

template<size_t Size>
inline bool CharBuffer<Size>::operator==(const CharBuffer& other) const {
	return !strcmp(_data, other._data);
}

template<size_t Size>
inline bool CharBuffer<Size>::operator!=(const CharBuffer& other) const
{
	return !(*this == other);
}

template<size_t Size>
inline CharBuffer<Size>::operator std::string() const {
	return std::string{_data};
}

template<size_t Size>
inline const char* CharBuffer<Size>::data() const {
	return _data;
}

template<size_t Size>
inline const size_t CharBuffer<Size>::size() {
	return Size;
}

template<size_t Size>
std::ostream& operator<<(std::ostream& out, const CharBuffer<Size>& buffer) {
	return (out << buffer._data);
}
