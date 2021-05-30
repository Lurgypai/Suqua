#pragma once
#include "Tick.h"
#include <unordered_map>
#include "ByteStream.h"
#include "Packet.h"
#include "SyncState.h"

//stores history, and gets ready to send it

class SyncSystem {
public:
	void storeCurrentState(Tick gameTime);
    void overrideState(Tick gameTime);

	void writeStatePacket(ByteStream& stream, Tick gameTime);

	void resyncStatePacket(ByteStream& stream, Game& game);
    const std::unordered_map<Tick, SyncState>& getStates() const;
	
private:
	Tick lastStoredTime;
	std::unordered_map<Tick, SyncState> states;
};

/*
STATE STORE ORDERING
Game Time
Number of Stored Entities
	Entity:
		Online Id
		NDC
		Controller (if applicable)
*/
