#pragma once
#include <vector>
#include <string>
#include "NetworkTypes.h"

struct NetworkEntityDescriptor {
    std::string tag;
    std::vector<NetworkId> netIds;
};
