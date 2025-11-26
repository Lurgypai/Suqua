#include "UUID.h"

#include <format>
#include <random>
#include <iomanip>
#include <cstdint>

namespace Suqua {
UUID UUID::GenerateUUID() {
    static std::random_device r{};
    static std::default_random_engine e{r()};
    static std::uniform_int_distribution<std::uint64_t> dist{};

    UUID uuid;
    uuid.data_[0] = dist(e);
    uuid.data_[1] = dist(e);

    return uuid;
}

std::string UUID::str() const {
    return std::format("{:016d}-{:016d}", data_[0], data_[1]);
}
}

std::ostream& Suqua::operator<<(std::ostream& left, const Suqua::UUID& value) {
    return left <<
       std::hex << std::setw(16) << std::setfill('0') << value.data_[0]
       << '-' 
       << std::hex << std::setw(16) << std::setfill('0') << value.data_[1];
}
