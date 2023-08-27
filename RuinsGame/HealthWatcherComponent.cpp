#include "HealthWatcherComponent.h"
#include "HealthComponent.h"
#include "EntityBaseComponent.h"

HealthWatcherComponent::HealthWatcherComponent(EntityId id_) : id{ id_ } {}

void HealthWatcherComponent::update() {
	auto healthComp = EntitySystem::GetComp<HealthComponent>(parentId);
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	if (healthComp->getHealth() != 0) baseComp->isActive = true;
	else baseComp->isActive = false;
}