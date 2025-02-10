#include "Director.h"

#include "EntitySpawnSystem.h"
#include "PhysicsComponent.h"
#include "EntityBaseComponent.h"
#include "RandomUtil.h"

#include "DebugIO.h"

using UUID = Suqua::UUID;

void Director::load(World& world_, Scene& scene, const std::string& spawnLevel) {
    exitRadius = 50.f;
    spawnDelay = 5.f;
    world = &world_;
    currLevelId = spawnLevel;

    for(const auto& level : world->getLevels()) {
        if(level.first == spawnLevel) continue;
        levels.push_back(level.first);
    }

    exitId = scene.addEntities(1)[0];
    EntitySystem::MakeComps<NetworkDataComponent>(1, &exitId,
            UUID::GenerateUUID(),
            NetworkDataComponent::Owner::local_only);
    EntitySystem::MakeComps<PhysicsComponent>(1, &exitId,
            Vec2f{ 0.f, 0.f },
            Vec2f{ 1.f, 1.f },
            false,
            false );

    beginFinished();
}

void Director::update(Scene& scene, float delta) {
    timeSinceLastAction += delta;
    switch(state) {
    case State::spawning:
        doSpawning(scene);
        break;
    case State::waiting:
        doWaiting();
        break;
    case State::finished:
        doFinished();
        break;
    }

    /* DEBUGIO */
    std::string stateStr = "";
    switch(state) {
        case State::spawning: stateStr = "spawning"; break;
        case State::waiting: stateStr = "waiting"; break;
        case State::finished: stateStr = "finished"; break;
    }
    DebugIO::setLine(6, "Director State: " + stateStr);
    DebugIO::setLine(7, "timeSinceLastAction: " + std::to_string(timeSinceLastAction));
    DebugIO::setLine(8, "clustersRemaining: " + std::to_string(clustersRemaining));
    /***********/
}

EntityId Director::getExitId() {
    return exitId;
}

void Director::addPlayer(EntityId id) {
    players.push_back(id);
}

void Director::removePlayer(EntityId id) {
    for(auto iter = players.begin(); iter != players.end(); ++iter) {
        if(*iter == id) {
            players.erase(iter);
            return;
        }
    }
}

void Director::doSpawning(Scene& scene) {
    if(timeSinceLastAction < spawnDelay) return;
    timeSinceLastAction -= spawnDelay;
    spawnCluster(scene);
    --clustersRemaining;
    if(clustersRemaining == 0) {
        state = State::waiting;
    }

    removeDeadEnemies();
}

void Director::doWaiting() {
    removeDeadEnemies();
    if(!remainingEnemies.empty()) return;
    beginFinished();
}

void Director::beginFinished() {
    state = State::finished;
    auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(exitId);
    const auto& level = world->getLevel(currLevelId);
    for(const auto& entity : level.getEntities()) {
        if(entity.id != "Exit") continue;
        physicsComp->teleport(entity.pos);
        break;
    }
    auto* baseComp = EntitySystem::GetComp<EntityBaseComponent>(exitId);
    baseComp->isActive = true;
}

void Director::doFinished() {
    bool allInRange = true;
    auto* exitPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(exitId);

    for(auto playerId : players) {
        auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(playerId);
        float dist = physicsComp->position().distance(exitPhysicsComp->position());
        if(dist > exitRadius) {
            allInRange = false;
            break;
        }
    }

    if(!allInRange) return;

    world->getLevel(currLevelId).deactivate();

    // select the next stage
    std::string nextLevel = currLevelId;
    while(nextLevel == currLevelId) {
        int index = randInt(0, levels.size() - 1);
        nextLevel = levels[index];
    }
    currLevelId = nextLevel;
    auto& level = world->getLevel(currLevelId);
    level.activate();

    // teleport players, initialize cluster spawn positions
    activeSpawnPositions.clear();
    for(const auto& entity : level.getEntities()) {
        if(entity.id == "EnemySpawn") {
            activeSpawnPositions.push_back(entity.pos);
            continue;
        }

        if(entity.id != "PlayerSpawn") continue;

        for(auto playerId : players) {
            auto* physics = EntitySystem::GetComp<PhysicsComponent>(playerId);
            physics->teleport(entity.pos);
        }
    }

    // deactiveate the exit
    auto* base = EntitySystem::GetComp<EntityBaseComponent>(exitId);
    base->isActive = false;
    // set to spawning
    state = State::spawning;
    timeSinceLastAction = 0;
    ++stage;
    clustersRemaining = stage + 2;
}

void Director::spawnCluster(Scene& scene) {
    int index = randInt(0, activeSpawnPositions.size() - 1);
    const auto& spawnPos = activeSpawnPositions[index];
    EntityId id = EntitySpawnSystem::SpawnEntity("enemy.basic", scene, spawnPos, NetworkDataComponent::Owner::local_shared);
    remainingEnemies.insert(id);
}


void Director::removeDeadEnemies() {
    for(auto iter = remainingEnemies.begin(); iter != remainingEnemies.end();) {
        auto enemyId = *iter;
        auto* base = EntitySystem::GetComp<EntityBaseComponent>(enemyId);
        if(base->isActive) {
            ++iter;
            continue;
        }

        base->isDead = true;
        iter = remainingEnemies.erase(iter);
    }
}
