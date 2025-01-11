#pragma once
#include "ComponentMacros.h"
#include "AABB.h"
#include <unordered_set>


class ActiveEntityZoneComponent {
    CompMembers(ActiveEntityZoneComponent);
public:
    
    bool isWithinActiveBox(const Vec2f& pos);
    bool isOutsideInactiveBox(const Vec2f& pos);

    void addTriggered(EntityId id);
    void removeTriggered(EntityId id);
    bool hasTriggered(EntityId id);

    void update();

    AABB activeBox;
    AABB inactiveBox;
private:
    // track entities that triggered this box
    std::unordered_set<EntityId> currentEntities;
};
