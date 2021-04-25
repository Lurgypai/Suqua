#include "..\header\SyncState.h"
#include "OnlineComponent.h"
#include "OnlineSystem.h"

SyncState::SyncState(Tick gameTime_) :
	gameTime{gameTime_}
{
	for (auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
		auto* controller = EntitySystem::GetComp<ControllerComponent>(data.getId());

		states.emplace(data.getId(), State{ data, controller ? std::optional<ControllerComponent>{*controller} : std::nullopt });
	}
}

SyncState::SyncState() {}

bool SyncState::operator==(const SyncState& other) {
	return gameTime == other.gameTime && states == other.states;
}

void SyncState::serialize(ByteStream& stream) {
	stream << gameTime;
	stream << states.size();
	for (auto&& state : states) {
		auto* online = EntitySystem::GetComp<OnlineComponent>(state.first);
		stream << online->getId();
		state.second.data.serializeForNetwork(stream);
		stream << state.second.cont.has_value();
		if (state.second.cont) {
			auto& controller = state.second.cont->getController();
			stream << controller.getPrevState();
			stream << controller.getState();
			stream << controller.pointerPos;
			stream << controller.mouseScroll;
		}
	}
}

void SyncState::unserialize(ByteStream& stream, const OnlineSystem& online) {
	stream >> gameTime;
	size_t size;
	stream >> size;
	for (size_t i = 0; i != size; ++i) {
		NetworkId netId;
		stream >> netId;

		EntityId id = online.getEntity(netId);
		State s{};
		s.data.unserialize(stream);
		bool has_cont;
		stream >> has_cont;
		if (has_cont) {
			ControllerState controllerState;
			ControllerState prevControllerState;
			Controller cont;
			
			stream >> prevControllerState;
			stream >> controllerState;
			stream >> cont.pointerPos;
			stream >> cont.mouseScroll;

			cont.setState(prevControllerState);
			cont.setState(controllerState);
		}

		states[id] = s;
	}
}

Tick SyncState::getGameTime() const {
	return gameTime;
}