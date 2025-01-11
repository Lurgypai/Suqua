#pragma once
#include "ComponentMacros.h"

class EntitySpawnComponent {
    CompMembers(EntitySpawnComponent);
public:
    enum class SpawnMode {
        enter, // when this spawner enters the spawn zone
        delay, // every n ticks, unimplemented
        trigger, // when triggered
    };

    void update();
    std::string spawnTag;
    SpawnMode mode;
private:
};


