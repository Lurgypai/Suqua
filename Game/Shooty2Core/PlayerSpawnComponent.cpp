#include "PlayerSpawnComponent.h"

PlayerSpawnComponent::PlayerSpawnComponent(EntityId id_) :
    id{ id_ },
    spawnPositions{}
{}

const Vec2f& PlayerSpawnComponent::getSpawnPos(const std::string& levelId) const {
    return spawnPositions.at(levelId);
};

void PlayerSpawnComponent::insertSpawnPos(const std::string& levelId, const Vec2f& pos) {
    spawnPositions.emplace(levelId, pos);
}
