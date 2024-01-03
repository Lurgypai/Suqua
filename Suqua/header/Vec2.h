#pragma once
#include <ostream>
#include <cstdint>
#include <cmath>

template<typename T>
struct Vec2 {
	T x, y;
	Vec2();
	Vec2(T x, T y);
	template<typename D>
	Vec2(const Vec2<D>& other);
	Vec2 operator+(const Vec2& other);
	Vec2 operator-(const Vec2& other);
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

	void operator+=(const Vec2& other);
	void operator-=(const Vec2& other);
	bool operator==(const Vec2& other) const;
	bool operator!=(const Vec2& other) const;
	T distance(const Vec2& other) const;
	T magn() const;
	float angle() const;
	void angle(float newAngle);
	Vec2 round() const;
	Vec2 ceil() const;
	Vec2 floor() const;
	Vec2 scale(const T& value) const;

	Vec2 abs();
	Vec2 norm();
};

template<typename T>
Vec2<T> operator-(Vec2<T> us) {
	return { -us.x, -us.y };
}

template<typename T>
Vec2<T>::Vec2() : x{}, y{} {}

template<typename T>
Vec2<T>::Vec2(T x_, T y_) :
	x{ x_ },
	y{ y_ }
{}

template<typename T>
template<typename D>
Vec2<T>::Vec2(const Vec2<D>& other) {
	x = static_cast<T>(other.x);
	y = static_cast<T>(other.y);
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator+(const V& other) const {
	return Vec2<T>{x + other.x, y + other.y};
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator-(const V& other) const {
	return Vec2<T>{x - other.x, y - other.y};
}



template<typename T>
template<typename V>
inline void Vec2<T>::operator+=(const V& other) {
	x += other;
	y += other;
}

template<typename T>
template<typename V>
inline void Vec2<T>::operator-=(const V& other) {
	x -= other;
	y -= other;
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator*(const V& other) {
	return Vec2<T>{x* other, y* other};
}

template<typename T>
template<typename V>
inline Vec2<T> Vec2<T>::operator/(const V& other) {
	return Vec2<T>{x / other, y / other};
}

template<typename T>
template<typename V>
inline void Vec2<T>::operator*=(const V& other) {
	x *= other;
	y *= other;
}

template<typename T>
template<typename V>
inline void Vec2<T>::operator/=(const V& other) {
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
void Vec2<T>::operator+=(const Vec2<T>& other) {
	x += other.x;
	y += other.y;
}

template<typename T>
void Vec2<T>::operator-=(const Vec2<T>& other) {
	x -= other.x;
	y -= other.y;
}

template<typename T>
inline bool Vec2<T>::operator==(const Vec2& other) const {
	return x == other.x && y == other.y;
}

template<typename T>
inline bool Vec2<T>::operator!=(const Vec2& other) const {
	return x != other.x || y != other.y;
}

template<typename T>
T Vec2<T>::distance(const Vec2<T>& other) const {
	T xDiff = x - other.x;
	T yDiff = y - other.y;

	return sqrt(xDiff * xDiff + yDiff * yDiff);
}

template<typename T>
T Vec2<T>::magn() const {
	return sqrt(x * x + y * y);
}

template<typename T>
float Vec2<T>::angle() const {
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}

template<typename T>
void Vec2<T>::angle(float newAngle) {
	float m = magn();
	x = std::cos(newAngle) * m;
	y = std::sin(newAngle) * m;
}

template<typename T>
inline Vec2<T> Vec2<T>::round() const {
	return Vec2<T>{std::round(x), std::round(y)};
}

template<typename T>
inline Vec2<T> Vec2<T>::ceil() const {
	return Vec2<T>{std::ceil(x), std::ceil(y)};
}

template<typename T>
inline Vec2<T> Vec2<T>::floor() const {
	return Vec2<T>{std::floor(x), std::floor(y)};
}

template<typename T>
inline Vec2<T> Vec2<T>::scale(const T& value) const {
	return Vec2<T>{x* value, y* value};
}


template<typename T>
inline Vec2<T> Vec2<T>::abs() {
	return { std::abs(x), std::abs(y) };
}

template<typename T>
inline Vec2<T> Vec2<T>::norm() {
	T m = magn();
	if (m == 0)
		return Vec2<T>{0, 0};
	return Vec2<T>{x / m, y / m};
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec2<T>& vec) {
	return os << vec.x << ", " << vec.y;
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int32_t>;
