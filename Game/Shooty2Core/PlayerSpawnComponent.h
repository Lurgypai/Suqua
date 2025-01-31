#pragma once
#include "ComponentMacros.h"

#include <string>
#include <unordered_map>

#include "Vec2.h"

class PlayerSpawnComponent {
    CompMembers(PlayerSpawnComponent);
public:
    PlayerSpawnComponent(EntityId id_);

    const Vec2f& getSpawnPos(const std::string& levelId) const;
    void insertSpawnPos(const std::string& levelId, const Vec2f& pos);
private:
    std::unordered_map<std::string, Vec2f> spawnPositions;
};
