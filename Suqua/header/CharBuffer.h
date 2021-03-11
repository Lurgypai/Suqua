#pragma once
#include <cstring>
#include <ostream>
#include <array>

//excludes the null terminator in the underlying data representation when copying into (zeroes remaining data for sanity)
//holds block of memory directly, as opposed to a pointer, for easy serialization (read write directly into)
template<size_t Size>
class CharBuffer {
public:
	CharBuffer();
	CharBuffer(const CharBuffer& other);
	CharBuffer(const std::string& s);
	template<size_t InSize>
	constexpr explicit CharBuffer(const char (&input)[InSize]);

	CharBuffer& operator=(const CharBuffer& other);
	CharBuffer& operator=(const std::string& s);
	bool operator==(const CharBuffer& other) const;
	bool operator!=(const CharBuffer& other) const;
	char& operator[](size_t pos);

	std::string str() const;

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
inline CharBuffer<Size>::CharBuffer(const CharBuffer<Size>& other) :
	_data{}
{
	for (auto i = 0; i != Size; ++i) {
		_data[i] = other._data[i];
	}
}

template<size_t Size>
inline CharBuffer<Size>::CharBuffer(const std::string& s) :
	_data{}
{
	if (s.size() <= Size) {
		size_t i = 0;

		while (i != s.size()) {
			_data[i] = s[i];
			++i;
		}
		//clean up remaining space
		if (i != Size) {
			for (auto j = i; j != Size; ++j) {
				_data[j] = '\0';
			}
		}
	}
	else {
		throw std::exception{};
	}
}

template<size_t Size>
template<size_t InSize>
inline constexpr CharBuffer<Size>::CharBuffer<Size>(const char (&input)[InSize]) :
	_data{}
{
	if (InSize > Size)
		throw std::out_of_range{""};

	constexpr size_t copy_size = Size > InSize ? InSize : Size;

	for (auto i = 0; i != copy_size; ++i) {
		_data[i] = input[i];
	}
}

template<size_t Size>
inline CharBuffer<Size>& CharBuffer<Size>::operator=(const CharBuffer<Size>& other) {
	for (auto i = 0; i != Size; ++i) {
		_data[i] = other._data[i];
	}
	return *this;
}

template<size_t Size>
inline CharBuffer<Size>& CharBuffer<Size>::operator=(const std::string& s)
{
	if (s.size() <= Size) {
		size_t i = 0;

		while (i != s.size()) {
			_data[i] = s[i];
			++i;
		}
		//clean up remaining space
		if (i != Size) {
			for (auto j = i; j != Size; ++j) {
				_data[j] = '\0';
			}
		}
	}
	else {
		throw std::exception{};
	}
	return *this;
}

template<size_t Size>
inline bool CharBuffer<Size>::operator==(const CharBuffer<Size>& other) const {
	for (auto i = 0; i != Size; ++i) {
		if (_data[i] != other._data[i])
			return false;
	}
	return true;
}

template<size_t Size>
inline bool CharBuffer<Size>::operator!=(const CharBuffer<Size>& other) const
{
	return !(*this == other);
}

template<size_t Size>
inline std::string CharBuffer<Size>::str() const {
	std::string ret{};
	ret.reserve(Size);
	size_t i = 0;
	while (i != Size && _data[i] != '\0') {
		ret += _data[i];
		++i;
	}
	return ret;
}

template<size_t Size>
inline const char* CharBuffer<Size>::data() const {
	return _data.data();
}

template<size_t Size>
inline const size_t CharBuffer<Size>::size() {
	return Size;
}

template<size_t Size>
inline char& CharBuffer<Size>::operator[](size_t pos) {
	return _data[pos];
}

template<size_t Size>
std::ostream& operator<<(std::ostream& out, const CharBuffer<Size>& buffer) {
	return (out << buffer.str());
}
