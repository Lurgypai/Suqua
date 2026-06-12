#include "PlayerSpawnComponent.h"
#include <stdexcept>

PlayerSpawnComponent::PlayerSpawnComponent(EntityId id_) :
    id{ id_ },
    spawnPositions{}
{}

const Vec2f& PlayerSpawnComponent::getSpawnPos(const std::string& levelId) const {
    auto iter = spawnPositions.find(levelId);
    if(iter == spawnPositions.end()) throw std::runtime_error{std::format(
            "PlayerSpawnComponent: Unable to find levelId \"{}\" spawn list",
            levelId
            )};
    return spawnPositions.at(levelId);
};

void PlayerSpawnComponent::insertSpawnPos(const std::string& levelId, const Vec2f& pos) {
    spawnPositions.emplace(levelId, pos);
}
