#pragma once
#include <array>
#include <cstdint>

class UUID {
public:
    static UUID GenerateUUID();

    bool operator==(const UUID& other) const = default;
    bool operator!=(const UUID& other) const = default;
private:
    std::array<std::uint64_t, 2> data_;
};
