#pragma once 

#include <unordered_set>
#include <vector>

#include "World.h"
#include "Scene.h"


/* TODO
 * enemy cluster spawning
 * network sync
 * ctor
 *
 */

class Director {
public:
    void load(World& world_, Scene& scene, const std::string& spawnLevel);
    void update(Scene& scene, float delta);
    EntityId getExitId();

    void addPlayer(EntityId id);
    void removePlayer(EntityId id);
private:
    World* world;

    enum class State {
        spawning,
        waiting,
        finished
    };


    State state;
    void doSpawning(Scene& scene);
    void doWaiting();
    void beginFinished();
    void doFinished();

    void spawnCluster(Scene& scene);
    void removeDeadEnemies();

    std::vector<std::string> levels;
    std::string currLevelId;
    int stage;

    // spawning data
    float timeSinceLastAction;
    float spawnDelay;
    int clustersRemaining;
    std::set<EntityId> remainingEnemies;
    std::vector<Vec2f> activeSpawnPositions;

    //exit
    EntityId exitId;
    float exitRadius;

    std::vector<EntityId> players;
};
