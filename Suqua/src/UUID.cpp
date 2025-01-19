#include "UUID.h"

#include <random>
#include <iomanip>
#include <cstdint>

UUID UUID::GenerateUUID() {
    static std::random_device r{};
    static std::default_random_engine e{r()};
    static std::uniform_int_distribution<std::uint64_t> dist{};

    UUID uuid;
    uuid.data_[0] = dist(e);
    uuid.data_[1] = dist(e);

    return uuid;
}

std::ostream& operator<<(std::ostream& left, const UUID& value) {
    return left <<
       std::hex << std::setw(16) << std::setfill('0') << value.data_[0]
       << '-' 
       << std::hex << std::setw(16) << std::setfill('0') << value.data_[1];
}
