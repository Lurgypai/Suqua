#pragma once

#include <cstdint>

enum AIState : std::uint8_t {
    idle,
    walking,
    following,
    attacking,
};
