#pragma once
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <exception>

template<typename T>
T s_hton(T value);

template<typename T>
T s_ntoh(T source);

template<typename T>
T s_htonl(T value);

template<typename T>
T s_htonll(T value);

template<typename T>
T s_nthol(T source);

template<typename T>
T s_ntholl(T source);

template<typename T>
inline T s_hton(T value) {
	switch (sizeof(T))
	{
	case 1:
		return value;
	case 4:
		return s_htonl(value);
	case 8:
		return s_htonll(value);
	default:
		//to big
		throw std::exception{};
	}
}

template<typename T>
inline T s_ntoh(T source) {
	switch (sizeof(T))
	{
	case 1:
		return source;
	case 4:
		return s_ntohl(source);
	case 8:
		return s_ntohll(source);
	default:
		break;
	}
}

//--------------------------HTON---------------------------
template<typename T>
inline T s_htonl(T inValue) {

	uint32_t value;
	std::memcpy(&value, &inValue, sizeof(inValue));

	uint8_t retPtr[4]{};
	retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
	retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
	retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
	retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);

	T ret{};
	memcpy(&ret, retPtr, sizeof(ret));
	return ret;
}

template<typename T>
inline T s_htonll(T inValue) {

	uint64_t value;
	std::memcpy(&value, &inValue, sizeof(inValue));
	
	uint8_t retPtr[8]{};

	retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
	retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
	retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
	retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);
	retPtr[4] = static_cast<uint8_t>((value >> 32) & 0xFFU);
	retPtr[5] = static_cast<uint8_t>((value >> 40) & 0xFFU);
	retPtr[6] = static_cast<uint8_t>((value >> 48) & 0xFFU);
	retPtr[7] = static_cast<uint8_t>((value >> 56) & 0xFFU);

	T ret{};
	memcpy(&ret, retPtr, sizeof(ret));
	return ret;
}

//---------------------------HTON SPECIALIZATIONS---------------------
template<>
inline uint32_t s_hton<uint32_t>(uint32_t value) {

	uint8_t retPtr[4]{};
	retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
	retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
	retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
	retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);

	uint32_t ret{};
	memcpy(&ret, retPtr, sizeof(ret));
	return ret;
}

template<>
inline int32_t s_hton<int32_t>(int32_t value) {
	uint8_t retPtr[4]{};
	retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
	retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
	retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
	retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);

	int32_t ret{};
	memcpy(&ret, retPtr, sizeof(ret));
	return ret;
}

template<>
inline uint64_t s_hton<uint64_t>(uint64_t value) {

	uint8_t retPtr[8]{};

	retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
	retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
	retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
	retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);
	retPtr[4] = static_cast<uint8_t>((value >> 32) & 0xFFU);
	retPtr[5] = static_cast<uint8_t>((value >> 40) & 0xFFU);
	retPtr[6] = static_cast<uint8_t>((value >> 48) & 0xFFU);
	retPtr[7] = static_cast<uint8_t>((value >> 56) & 0xFFU);

	uint64_t ret{};
	memcpy(&ret, retPtr, sizeof(ret));
	return ret;
}

template<>
inline int64_t s_hton<int64_t>(int64_t value) {

	uint8_t retPtr[8]{};

	retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
	retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
	retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
	retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);
	retPtr[4] = static_cast<uint8_t>((value >> 32) & 0xFFU);
	retPtr[5] = static_cast<uint8_t>((value >> 40) & 0xFFU);
	retPtr[6] = static_cast<uint8_t>((value >> 48) & 0xFFU);
	retPtr[7] = static_cast<uint8_t>((value >> 56) & 0xFFU);

	int64_t ret{};
	memcpy(&ret, retPtr, sizeof(ret));
	return ret;
}

//---------------------------NTOH---------------------------

template<typename T>
inline T s_ntohl(T source) {
	uint8_t sourcePtr[4]{};
	memcpy(sourcePtr, &source, sizeof(source));

	uint32_t ret32 = 
		(static_cast<uint32_t>(sourcePtr[0]) << 0) |
		(static_cast<uint32_t>(sourcePtr[1]) << 8) |
		(static_cast<uint32_t>(sourcePtr[2]) << 16) |
		(static_cast<uint32_t>(sourcePtr[3]) << 24);

	T ret;
	std::memcpy(&ret, &ret32, sizeof(ret));
	return ret;
}

template<typename T>
inline T s_ntohll(T source) {
	uint8_t sourcePtr[8]{};
	memcpy(sourcePtr, &source, sizeof(source));
	uint64_t ret64 =
		(static_cast<uint64_t>(sourcePtr[0]) << 0) |
		(static_cast<uint64_t>(sourcePtr[1]) << 8) |
		(static_cast<uint64_t>(sourcePtr[2]) << 16) |
		(static_cast<uint64_t>(sourcePtr[3]) << 24) |
		(static_cast<uint64_t>(sourcePtr[4]) << 32) |
		(static_cast<uint64_t>(sourcePtr[5]) << 40) |
		(static_cast<uint64_t>(sourcePtr[6]) << 48) |
		(static_cast<uint64_t>(sourcePtr[7]) << 56);

	T ret;
	std::memcpy(&ret, &ret64, sizeof(ret));
	return ret;
}

//--------------------------NTOH SPECIALIZATION-------------------------
template<>
inline uint32_t s_ntoh<uint32_t>(uint32_t source) {
	uint8_t sourcePtr[4]{};
	memcpy(sourcePtr, &source, sizeof(source));

	return
		(static_cast<uint32_t>(sourcePtr[0]) << 0) |
		(static_cast<uint32_t>(sourcePtr[1]) << 8) |
		(static_cast<uint32_t>(sourcePtr[2]) << 16) |
		(static_cast<uint32_t>(sourcePtr[3]) << 24);
}

template<>
inline int32_t s_ntoh<int32_t>(int32_t source) {
	uint8_t sourcePtr[4]{};
	memcpy(sourcePtr, &source, sizeof(source));

	return
		(static_cast<int32_t>(sourcePtr[0]) << 0) |
		(static_cast<int32_t>(sourcePtr[1]) << 8) |
		(static_cast<int32_t>(sourcePtr[2]) << 16) |
		(static_cast<int32_t>(sourcePtr[3]) << 24);
}

template<>
inline uint64_t s_ntoh<uint64_t>(uint64_t source) {
	uint8_t sourcePtr[8]{};
	memcpy(sourcePtr, &source, sizeof(source));
	return
		(static_cast<uint64_t>(sourcePtr[0]) << 0) |
		(static_cast<uint64_t>(sourcePtr[1]) << 8) |
		(static_cast<uint64_t>(sourcePtr[2]) << 16) |
		(static_cast<uint64_t>(sourcePtr[3]) << 24) |
		(static_cast<uint64_t>(sourcePtr[4]) << 32) |
		(static_cast<uint64_t>(sourcePtr[5]) << 40) |
		(static_cast<uint64_t>(sourcePtr[6]) << 48) |
		(static_cast<uint64_t>(sourcePtr[7]) << 56);
}

template<>
inline int64_t s_ntoh<int64_t>(int64_t source) {
	uint8_t sourcePtr[8]{};
	memcpy(sourcePtr, &source, sizeof(source));
	return
		(static_cast<int64_t>(sourcePtr[0]) << 0) |
		(static_cast<int64_t>(sourcePtr[1]) << 8) |
		(static_cast<int64_t>(sourcePtr[2]) << 16) |
		(static_cast<int64_t>(sourcePtr[3]) << 24) |
		(static_cast<int64_t>(sourcePtr[4]) << 32) |
		(static_cast<int64_t>(sourcePtr[5]) << 40) |
		(static_cast<int64_t>(sourcePtr[6]) << 48) |
		(static_cast<int64_t>(sourcePtr[7]) << 56);
}
