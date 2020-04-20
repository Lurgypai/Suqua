#pragma once
#include <ostream>
#include <cstdint>

template<typename T>
struct Vec2 {
	T x, y;
	Vec2();
	Vec2(T x, T y);
	template<typename D>
	Vec2(const Vec2<D>& other);
	Vec2 operator+(const Vec2 & other);
	Vec2 operator-(const Vec2 & other);
	template<typename V>
	Vec2 operator+(const V& other) const;

	template<typename V>
	Vec2 operator-(const V& other) const;

	template<typename V>
	void operator+=(const V& other);

	template<typename V>
	void operator-=(const V& other);

	template<typename V>
	Vec2 operator*(const V& other);

	template<typename V>
	Vec2 operator/(const V& other);

	template<typename V>
	void operator*=(const V& other);

	template<typename V>
	void operator/=(const V& other);

	void operator+=(const Vec2 & other);
	void operator-=(const Vec2 & other);
	bool operator==(const Vec2 & other) const;
	bool operator!=(const Vec2 & other) const;
	T distance(const Vec2 & other);

	Vec2 abs();
};

template<typename T>
Vec2<T> operator-(Vec2<T> us) {
	return { -us.x, -us.y };
}

template<typename T>
Vec2<T>::Vec2() : x{}, y{} {}

template<typename T>
Vec2<T>::Vec2(T x_, T y_) :
	x{x_},
	y{y_}
{}

template<typename T>
template<typename D>
Vec2<T>::Vec2(const Vec2<D> & other) {
	x = static_cast<T>(other.x);
	y = static_cast<T>(other.y);
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator+(const V & other) const {
	return Vec2<T>{x + other.x, y + other.y};
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator-(const V & other) const {
	return Vec2<T>{x - other.x, y - other.y};
}



template<typename T>
template<typename V>
inline void Vec2<T>::operator+=(const V & other) {
	x += other;
	y += other;
}

template<typename T>
template<typename V>
inline void Vec2<T>::operator-=(const V & other) {
	x -= other;
	y -= other;
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator*(const V & other) {
	return Vec2<T>{x * other, y * other};
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator/(const V & other) {
	return Vec2<T>{x / other, y / other};
}

template<typename T>
template<typename V>
inline void Vec2<T>::operator*=(const V & other) {
	x *= other;
	y *= other;
}

template<typename T>
template<typename V>
inline void Vec2<T>::operator/=(const V & other) {
	x /= other;
	y /= other;
}

template<typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T>& other) {
	return Vec2<T>{x + other.x, y + other.y};
}

template<typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T>& other) {
	return Vec2<T>{x - other.x, y - other.y};
}

template<typename T>
void Vec2<T>::operator+=(const Vec2<T> & other) {
	x += other.x;
	y += other.y;
}

template<typename T>
void Vec2<T>::operator-=(const Vec2<T> & other) {
	x -= other.x;
	y -= other.y;
}

template<typename T>
inline bool Vec2<T>::operator==(const Vec2 & other) const {
	return x == other.x && y == other.y;
}

template<typename T>
inline bool Vec2<T>::operator!=(const Vec2 & other) const {
	return x != other.x || y != other.y;
}

template<typename T>
T Vec2<T>::distance(const Vec2<T> & other) {
	T xDiff = x - other.x;
	T yDiff = y - other.y;

	return sqrt(xDiff * xDiff + yDiff * yDiff);
}

template<typename T>
inline Vec2<T> Vec2<T>::abs() {
	return {std::abs(x), std::abs(y)};
}

template<typename T>
std::ostream & operator<<(std::ostream & os, const Vec2<T> & vec) {
	return os << vec.x << ", " << vec.y;
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int32_t>;