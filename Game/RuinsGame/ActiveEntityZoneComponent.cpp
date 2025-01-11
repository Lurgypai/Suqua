#include "ActiveEntityZoneComponent.h"
#include "PhysicsComponent.h"
#include "EntitySystem.h"

ActiveEntityZoneComponent::ActiveEntityZoneComponent(EntityId id_) :
    id{id_}
{}

void ActiveEntityZoneComponent::update() {
    auto* pos = EntitySystem::GetComp<PhysicsComponent>(id);
    activeBox.center(pos->position());
    inactiveBox.center(pos->position());
}

bool ActiveEntityZoneComponent::isWithinActiveBox(const Vec2f& pos) {
    return activeBox.contains(pos);
}

bool ActiveEntityZoneComponent::isOutsideInactiveBox(const Vec2f& pos) {
    return !inactiveBox.contains(pos);
}

void ActiveEntityZoneComponent::addTriggered(EntityId id) {
    currentEntities.emplace(id);
}

void ActiveEntityZoneComponent::removeTriggered(EntityId id) {
    currentEntities.erase(id);
}

bool ActiveEntityZoneComponent::hasTriggered(EntityId id) {
    return currentEntities.find(id) != currentEntities.end();
}
