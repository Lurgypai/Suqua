#include "EntityGenerator.h"
#include "EntitySpawnFunctions.h"

#include "SpawnFunctionNotFoundException.h"

#include <unordered_map>

EntityId EntityGenerator::SpawnEntity(
        const std::string& tag,
        Scene& targetScene,
        const Vec2f& targetPos,
        NetworkDataComponent::Owner owner,
        const UUID& uuid) {
    if(EntityGenerator::SpawnFunctions.find(tag) == EntityGenerator::SpawnFunctions.end()) throw SpawnFunctionNotFoundException{tag};

    auto entities = EntityGenerator::SpawnFunctions.at(tag)(
            targetScene,
            targetPos,
            owner,
            uuid);

    return entities;
}

void EntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::SpawnFunctions.insert(std::make_pair("player.basic", SpawnPlayer));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("player.daemon", SpawnDaemon));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("enemy.basic", SpawnEnemy));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.player.basic", SpawnBulletPlayerBasic));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("bullet.enemy.basic", SpawnBulletEnemyBasic));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("world.teleportzone", SpawnTeleportZone));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("world.tile", SpawnTile));
}
