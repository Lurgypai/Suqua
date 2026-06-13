#pragma once

#include <cstdint>

enum class AIState : std::uint8_t {
    idle,
    walking,
    following,
    attacking,
};
