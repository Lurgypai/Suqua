#include "HitboxComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"

#include <stdexcept>

HitboxComponent::HitboxComponent(
        EntityId id_,
        const Vec2f& offset_,
        const Vec2f& res,
        bool dieOnHit_ ) :
	id{ id_ },
	offset{offset_},
	hitbox{{}, res},
	collisionsMax{ 1 },
	reHitDelay{ 0 },
	multipleHits{false},
    dieOnHit{dieOnHit_},

	hitEntities{},
	collisions{ 0 },
    active{true}
{}

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
    if(!active) return false;
	auto pair = hitEntities.find(entityId);
	return pair == hitEntities.end() || pair->second == 0;
}


void HitboxComponent::activate() {
    active = true;
	collisions = 0;
	for (auto& [_, hits] : hitEntities) {
		hits = 0;
	}
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	hitbox.pos = posComp->getPos() + offset;
}

void HitboxComponent::deactivate() {
    active = false;
    if(!dieOnHit) return;
    auto ebc = EntitySystem::GetComp<EntityBaseComponent>(id);
    ebc->isDead = true;
}

TeamComponent::TeamId HitboxComponent::getTeamId() const
{
	auto teamComponent = EntitySystem::GetComp<TeamComponent>(id);
    if(teamComponent == nullptr) throw std::runtime_error{"HitboxComponent: Missing Team Component"};
	return teamComponent->teamId;
}
