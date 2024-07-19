#pragma once
#include "ComponentMacros.h"
#include "Vec2.h"

//waits to activate an entity till they're on screen, de-activates them if they're outside the spawn zone

class ActiveEntityComponent {
    CompMembers(ActiveEntityComponent);
public:
    void update();
    void deactivate();
    void activate();
    void reset();

    Vec2f spawnPos;
private:
    bool canReset;
};
