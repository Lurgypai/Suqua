#pragma once
#include "Vec2.h"
#include "NetworkTypes.h"
#include "PlayerLC.h"
#include "PlayerData.h"
#define INPUT_QUEUE_SIZE 500
#include <vector>
#include <deque>

struct ClientCommand {
	Controller controllerState;
	Time_t when;
};

class ServerPlayerLC : public PlayerLC {
public:
	ServerPlayerLC( EntityId id_ = 0 );
	ServerPlayerLC(const ServerPlayerLC & other);
	Time_t getWhen() const;
	PlayerState getStateAt(Time_t gameTime);
	void setWhen(Time_t when_);
	void bufferInput(ClientCommand c);
	void update(Time_t gameTime);
	void runHitDetect(Time_t gameTime);
private:
	//the client time of the owning client that the last packet processed by the server was sent from the client
	Time_t when;
	std::vector<ClientCommand> inputs;
	std::deque<PlayerState> prevStates;
};