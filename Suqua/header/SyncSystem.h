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

	// If we have a "true" state for the current time, apply it immediately.
	void resync(Game& game);
	bool hasCurrentState(Game& game);

	// Handle incoming packets
	// if its for the future, stores it for later.
	// if its for the past, applies it and regenerates the current state.
	void resyncStatePacket(ByteStream& stream, Game& game);

    const std::unordered_map<Tick, SyncState>& getStates() const;
	bool hasState(Tick gameTime) const;
	
	//interpolates all interpolateable data
	void interpolate(Tick gameTime);
private:
	Tick lastStoredTime;
	//change this to a circular buffer
	std::unordered_map<Tick, SyncState> states;
	std::unordered_map<Tick, SyncState> futureStates;
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
