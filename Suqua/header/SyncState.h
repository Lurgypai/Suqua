#pragma once
#include "NetworkDataComponent.h"
#include "ControllerComponent.h"
#include "ByteStream.h"
#include "Tick.h"
#include "OnlineSystem.h"

#include <unordered_map>
#include <optional>

class SyncState {
private:
	struct State {
		NetworkDataComponent data;
		std::optional<ControllerComponent> cont;

        bool operator==(const State& other) const;
        bool operator!=(const State& other) const;
	};
public:
	SyncState(Tick gameTime_);
	SyncState();

	bool operator==(const SyncState& other) const;
	bool operator!=(const SyncState& other) const;

	void serialize(ByteStream& stream);
	void unserialize(ByteStream& stream, const OnlineSystem& online);

	Tick getGameTime() const;
    const std::unordered_map<EntityId, State>& getStates() const;
private:
	Tick gameTime;
	std::unordered_map<EntityId, State> states;
};
