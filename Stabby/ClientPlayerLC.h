#pragma once
#include "PlayerLC.h"
#include "Controller.h"
#include "PlayerData.h"
#include "NetworkTypes.h"
#include <deque>

#define BUFFER_SIZE 200

//input and value state at a specific time
struct TotalPlayerState {
	PlayerState plr;
	unsigned char in;
};

class ClientPlayerLC : public PlayerLC {
public:
	ClientPlayerLC(EntityId id_ = 0);
	void update(Time_t now, double timeDelta, const Controller & controller);
	//Repredict current loc using correct position at time when.
	void repredict(const PlayerState & state);
	std::string getHeadPath();
	Vec2f getCenter();
private:
	//client sender tick of last acknowledged input
	Time_t last;
	std::deque<TotalPlayerState> states;
};
