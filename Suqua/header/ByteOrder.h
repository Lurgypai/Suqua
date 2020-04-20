#pragma once
#include <cstddef>
#include <cstdint>

uint32_t s_htonl(uint32_t value);

uint32_t s_ntohl(uint32_t source);

uint64_t s_htonll(uint64_t value);

uint64_t s_ntohll(uint64_t source);

uint32_t s_htonf(float value);

float s_ntohf(uint32_t source);

uint64_t s_htond(double value);

double s_ntohd(uint64_t source);