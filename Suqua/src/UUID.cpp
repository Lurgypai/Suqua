#include "UUID.h"

#include <random>
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
