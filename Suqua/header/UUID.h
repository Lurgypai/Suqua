#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>

namespace Suqua {
class UUID;
std::ostream& operator<<(std::ostream& left, const Suqua::UUID& value);
}

class ByteStream;

namespace Suqua {
class UUID {
public:
    static UUID GenerateUUID();

    bool operator==(const UUID& other) const = default;
    bool operator!=(const UUID& other) const = default;
    std::string str() const;
private:
    std::array<std::uint64_t, 2> data_;

friend struct std::hash<UUID>;
friend class ::ByteStream;
friend std::ostream& operator<<(std::ostream& left, const UUID& value);
};
};

namespace std {
template <> struct hash<Suqua::UUID> {
    size_t operator()(const Suqua::UUID & x) const {
        std::hash<std::uint64_t> hash;
        size_t lhs = hash(x.data_[0]);
        size_t rhs = hash(x.data_[1]);
        // from https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
        if constexpr (sizeof(size_t) >= 8) {
            lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
        } else {
            lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        }
        return lhs;
    }
};
}

