#include "GameState.h"

void operator<<(MarkedStream& m, GameState g) {
	m << g.id;
	for (auto& pair : g.states) {
		pair.second.readInto(m);
	}
}

GameState::GameState(GameStateId id_) :
	acknowledged{false},
	id{id_},
	states{}
{}

void GameState::compareAndWrite(MarkedStream& m, const GameState& otherState) {
	m << id;
	for (auto& pair : states) {
		//if the other game state has this players online id, compare and write the state
		auto iter = otherState.states.find(pair.first);
		if (iter != otherState.states.end()) {
			pair.second.readInto(m, iter->second.state);
		}
	}
}

void GameState::acknowledge() {
	acknowledged = true;
}

bool GameState::getAcknowledged() const {
	return acknowledged;
}

GameStateId GameState::getId() const {
	return id;
}

void GameState::addPlayerState(const StatePacket& state) {
	states.emplace(state.id, state);
}
