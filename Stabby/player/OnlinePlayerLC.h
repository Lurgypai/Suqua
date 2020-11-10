#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "DynamicBitset.h"

#include "../player/PlayerStateComponent.h"
#include "network.h"

class OnlinePlayerLC {
public:
	OnlinePlayerLC(EntityId id_ = 0);
	EntityId getId() const;
	//add a position to move towards
	void interp(PlayerState st, const DynamicBitset& changedFields, Time_t when);
	void update(Time_t gameTime);
private:
	//the current and two previous positions. We are moving between the two previous positions.
	Vec2f previousPos[2];

	//stores a box created by the earliest previous positions, the onlineplayers position can't be updated outside of this.
	AABB positionBox;
	//when the current and two previous positions arrived.
	Time_t whens[2];
	//last time used
	Time_t lastClientTime;
	bool wasDead;
	EntityId id;
};