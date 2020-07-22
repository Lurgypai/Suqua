#pragma once
#include <unordered_map>
#include <cstdint>

#include "../network/packet/StatePacket.h"
#include "MarkedStream.h"

using GameStateId = uint64_t;

class GameState {
public:
	GameState(GameStateId id_);
	void compareAndWrite(MarkedStream& m, const GameState& otherState);
	friend void operator<<(MarkedStream& m, GameState g);
	void acknowledge();
	bool getAcknowledged() const;
	GameStateId getId() const;
	void addPlayerState(const StatePacket& state);
private:
	bool acknowledged;
	GameStateId id;
	std::unordered_map<NetworkId, StatePacket> states;
};

void operator<<(MarkedStream& m, GameState g);