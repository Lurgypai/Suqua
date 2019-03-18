#pragma once
#include "NetworkTypes.h"
#include "EntitySystem.h"
#include "Vec2.h"
#include "PlayerData.h"
#include "PlayerLC.h"
class OnlinePlayerLC {
public:
	OnlinePlayerLC(EntityId id_ = 0);
	void setNetId(NetworkId id_);
	NetworkId getNetId();
	EntityId getId() const;
	//add a position to move towards
	Vec2f getPos() const;
	Vec2f getVel() const;
	void interp(PlayerState st, Time_t when);
	void update(Time_t gameTime);
	int getActiveId();
	int getFacing();
	State getState();
	std::string getHeadPath();
private:
	//the current and two previous positions. We are moving between the two previous positions.
	Vec2f previousPos[3];
	//when the current and two previous positions arrived.
	Time_t whens[3];
	Vec2f pos;
	Vec2f vel;
	int attackId;
	int facing;
	NetworkId netId;
	EntityId id;
	State state;
};