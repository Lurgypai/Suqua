#include "HitboxComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"
#include "ParentComponent.h"

HitboxComponent::HitboxComponent(EntityId id_) :
	id{ id_ },
	hitbox {},
	offset{},
	collisions{ 0 },
	collisionsMax{ 1 },
	reHitDelay{ 0 },
	multipleHits{false},
	hitEntities{}
{}

EntityId HitboxComponent::getId() const {
	return id;
}

void HitboxComponent::update() {
	if (collisionsMax > 0) {
		if (collisions == collisionsMax) {
			deactivate();
			return;
		}
	}

	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	hitbox.pos = posComp->getPos() + offset;

	if (reHitDelay > 0) {
		for (auto& [_, delay] : hitEntities) {
			if (delay > 0) --delay;
		}
	}
}

bool HitboxComponent::addHitEntity(EntityId entityId) {
	auto pair = hitEntities.find(entityId);
	if (pair == hitEntities.end()) {
		hitEntities.emplace(entityId, reHitDelay);
		++collisions;
		return true;
	}

	if (pair->second == 0) {
		pair->second = reHitDelay;
		++collisions;
		return true;
	}

	return false;
}

bool HitboxComponent::canHitEntity(EntityId entityId) {
	auto pair = hitEntities.find(entityId);
	return pair == hitEntities.end() || pair->second == 0;
}


void HitboxComponent::activate()
{
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	baseComp->isActive = true;
	collisions = 0;
	for (auto& [_, hits] : hitEntities) {
		hits = 0;
	}
}

void HitboxComponent::deactivate()
{
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	baseComp->isActive = false;
}

TeamComponent::TeamId HitboxComponent::getTeamId() const
{
	auto teamComponent = EntitySystem::GetComp<TeamComponent>(id);
	if (teamComponent != nullptr) return teamComponent->teamId;

	//add a get root function to parent components
	auto parentComponent = EntitySystem::GetComp<ParentComponent>(id);
	auto parentTeamComponent = EntitySystem::GetComp<TeamComponent>(parentComponent->parentId);
	return parentTeamComponent->teamId;
}