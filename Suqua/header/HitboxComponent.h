#pragma once
#include "ComponentMacros.h"
#include "TeamComponent.h"
#include "AABB.h"
#include <unordered_map>

class HitboxComponent {
    CompMembers(HitboxComponent);
public:
	HitboxComponent(EntityId id_,
            const Vec2f& offset_,
            const Vec2f& res,
            bool dieOnHit_ );

	void update();
	// returns wether or not the entity was hit, and adds it as a hit entity
	bool addHitEntity(EntityId entityId);
	bool canHitEntity(EntityId entityId);

	void activate();
	void deactivate();

	TeamComponent::TeamId getTeamId() const;

	Vec2f offset;
	AABB hitbox;

	// max number of times we can hit something. 0 means infinite
	unsigned int collisionsMax;
	// number of frames before the same thing can be hit multiple times, negative means can't re-hit
	int reHitDelay;
	// wether we can hit the same thing multiple times
	bool multipleHits;

    // whether to remove this entity when the collisions reaches the max
    bool dieOnHit;
private:
	// map entities to remaining frames before they can be hit again
	std::unordered_map<EntityId, int> hitEntities;
	// how many times we've hit things
	unsigned int collisions;

    // whether this hitbox should do more collisions
    bool active;
};
