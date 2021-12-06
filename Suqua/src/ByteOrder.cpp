#include "ByteOrder.h"
#include <cstring>

/*
uint32_t s_hton(uint32_t value)
{

    uint8_t retPtr[4]{};
    retPtr[0] = static_cast<uint8_t>((value >> 0) & 0xFFU);
    retPtr[1] = static_cast<uint8_t>((value >> 8) & 0xFFU);
    retPtr[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
    retPtr[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);

    uint32_t ret{};
    memcpy(&ret, retPtr, sizeof(ret));
    return ret;
}

uint32_t s_ntoh(uint32_t source)
{
    uint8_t sourcePtr[4]{};
    memcpy(sourcePtr, &source, sizeof(source));

    return
        (static_cast<std::uint32_t>(sourcePtr[0]) << 0) |
        (static_cast<std::uint32_t>(sourcePtr[1]) << 8) |
        (static_cast<std::uint32_t>(sourcePtr[2]) << 16) |
        (static_cast<std::uint32_t>(sourcePtr[3]) << 24);
}

uint64_t s_hton(uint64_t value)
{
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

uint64_t s_ntoh(uint64_t source)
{
    uint8_t sourcePtr[8]{};
    memcpy(sourcePtr, &source, sizeof(source));
    return
        (static_cast<std::uint64_t>(sourcePtr[0]) << 0) |
        (static_cast<std::uint64_t>(sourcePtr[1]) << 8) |
        (static_cast<std::uint64_t>(sourcePtr[2]) << 16) |
        (static_cast<std::uint64_t>(sourcePtr[3]) << 24) |
        (static_cast<std::uint64_t>(sourcePtr[4]) << 32) |
        (static_cast<std::uint64_t>(sourcePtr[5]) << 40) |
        (static_cast<std::uint64_t>(sourcePtr[6]) << 48) |
        (static_cast<std::uint64_t>(sourcePtr[7]) << 56);
}

uint32_t s_hton(float value)
{
    uint8_t retPtr[4]{};
    uint32_t valuePtr{};
    memcpy(&valuePtr, &value, sizeof(valuePtr));

    retPtr[0] = static_cast<uint8_t>((valuePtr >> 0) & 0xFFU);
    retPtr[1] = static_cast<uint8_t>((valuePtr >> 8) & 0xFFU);
    retPtr[2] = static_cast<uint8_t>((valuePtr >> 16) & 0xFFU);
    retPtr[3] = static_cast<uint8_t>((valuePtr >> 24) & 0xFFU);

    uint32_t ret{};
    memcpy(&ret, retPtr, sizeof(ret));
    return ret;
}
float s_ntoh(uint32_t source)
{
    uint8_t sourcePtr[4]{};
    memcpy(sourcePtr, &source, sizeof(source));

    uint32_t ret =
        (static_cast<std::uint32_t>(sourcePtr[0]) << 0) |
        (static_cast<std::uint32_t>(sourcePtr[1]) << 8) |
        (static_cast<std::uint32_t>(sourcePtr[2]) << 16) |
        (static_cast<std::uint32_t>(sourcePtr[3]) << 24);

    float retFloat{};
    memcpy(&retFloat, &ret, sizeof(retFloat));
    return retFloat;
       
}

uint64_t s_hton(double value)
{
    uint8_t retPtr[8]{};
    uint64_t valuePtr{};
    memcpy(&valuePtr, &value, sizeof(valuePtr));

    retPtr[0] = static_cast<uint8_t>((valuePtr >> 0) & 0xFFU);
    retPtr[1] = static_cast<uint8_t>((valuePtr >> 8) & 0xFFU);
    retPtr[2] = static_cast<uint8_t>((valuePtr >> 16) & 0xFFU);
    retPtr[3] = static_cast<uint8_t>((valuePtr >> 24) & 0xFFU);
    retPtr[4] = static_cast<uint8_t>((valuePtr >> 32) & 0xFFU);
    retPtr[5] = static_cast<uint8_t>((valuePtr >> 40) & 0xFFU);
    retPtr[6] = static_cast<uint8_t>((valuePtr >> 48) & 0xFFU);
    retPtr[7] = static_cast<uint8_t>((valuePtr >> 56) & 0xFFU);

    uint64_t ret{};
    memcpy(&ret, retPtr, sizeof(ret));
    return ret;
}

/*
double s_ntoh(uint64_t source)
{
    uint8_t sourcePtr[8]{};
    memcpy(sourcePtr, &source, sizeof(source));
    uint64_t ret =
        (static_cast<std::uint64_t>(sourcePtr[0]) << 0) |
        (static_cast<std::uint64_t>(sourcePtr[1]) << 8) |
        (static_cast<std::uint64_t>(sourcePtr[2]) << 16) |
        (static_cast<std::uint64_t>(sourcePtr[3]) << 24) |
        (static_cast<std::uint64_t>(sourcePtr[4]) << 32) |
        (static_cast<std::uint64_t>(sourcePtr[5]) << 40) |
        (static_cast<std::uint64_t>(sourcePtr[6]) << 48) |
        (static_cast<std::uint64_t>(sourcePtr[7]) << 56);

    double retDouble{};
    memcpy(&retDouble, &ret, sizeof(retDouble));
    return retDouble;
}
*/