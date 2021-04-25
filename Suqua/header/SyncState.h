#include "NetworkDataComponent.h"
#include "ControllerComponent.h"
#include "ByteStream.h"
#include "Tick.h"
#include "OnlineSystem.h"

#include <unordered_map>
#include <optional>

class SyncState {
public:
	SyncState(Tick gameTime_);
	SyncState();

	bool operator==(const SyncState& other);
	bool operator!=(const SyncState& other);

	void serialize(ByteStream& stream);
	void unserialize(ByteStream& stream, const OnlineSystem& online);

	Tick getGameTime() const;
private:
	struct State {
		NetworkDataComponent data;
		std::optional<ControllerComponent> cont;
	};
	Tick gameTime;
	std::unordered_map<EntityId, State> states;
};