#pragma once
#include "EntitySystem.h"
#include "Vec2.h"

#include "../player/PlayerStateComponent.h"
#include "network.h"

class OnlinePlayerLC {
public:
	OnlinePlayerLC(EntityId id_ = 0);
	EntityId getId() const;
	//add a position to move towards
	void interp(PlayerState st, Time_t when);
	void update(Time_t gameTime);
private:
	//the current and two previous positions. We are moving between the two previous positions.
	Vec2f previousPos[3];

	//stores a box created by the earliest previous positions, the onlineplayers position can't be updated outside of this.
	AABB positionBox;
	//when the current and two previous positions arrived.
	Time_t whens[3];
	EntityId id;
};