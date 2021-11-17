#include "SyncState.h"
#include "OnlineComponent.h"
#include "OnlineSystem.h"
#include <unordered_map>

SyncState::SyncState(Tick gameTime_) :
	gameTime{gameTime_}
{
    if(EntitySystem::Contains<OnlineComponent>()) {
        for (auto&& online : EntitySystem::GetPool<OnlineComponent>()) {
			auto* data = EntitySystem::GetComp<NetworkDataComponent>(online.getId());
            ControllerComponent* controller = nullptr;
            if(EntitySystem::Contains<ControllerComponent>()) {
                controller = EntitySystem::GetComp<ControllerComponent>(data->getId());
            }

            states.emplace(data->getId(),
                    State{ *data,
                    controller ? std::optional<ControllerComponent>{*controller} :
                                 std::nullopt });
        }
    }
}

SyncState::SyncState(const SyncState& other) :
    gameTime{other.gameTime},
    states{other.states}
{}

SyncState::SyncState(SyncState&& other) :
    gameTime{other.gameTime},
    states{std::move(other.states)}
{}

SyncState& SyncState::operator=(SyncState& other) {
    gameTime = other.gameTime;
    states = other.states;
    return *this;
}

SyncState& SyncState::operator=(SyncState&& other) {
    gameTime = other.gameTime;
    states = std::move(other.states);
    return *this;
}

bool SyncState::operator==(const SyncState& other) const {
	return gameTime == other.gameTime && states == other.states;
}

bool SyncState::operator!=(const SyncState& other) const {
    return !((*this) == other);
}

void SyncState::serialize(ByteStream& stream) {
	stream << gameTime;
	stream << states.size();
	for (auto&& state : states) {
		auto* online = EntitySystem::GetComp<OnlineComponent>(state.first);
		stream << online->getNetId();
		state.second.data.serializeForNetwork(stream);
		stream << state.second.cont.has_value();
		if (state.second.cont) {
			auto& controller = state.second.cont->getController();
			controller.serialize(stream);
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
		State& s = states.at(id);
		s.data.unserialize(stream);
		bool has_cont;
		stream >> has_cont;
		if (has_cont) {
			s.cont->getController().unserialize(stream);
		}
	}
}

Tick SyncState::getGameTime() const {
	return gameTime;
}

const std::unordered_map<EntityId, SyncState::State>& SyncState::getStates() const {
    return states;
}

void SyncState::applyInput(const std::vector<NetworkId>& netIds, const OnlineSystem& online) {
	for (NetworkId netId : netIds) {
		EntityId entity = online.getEntity(netId);
		*EntitySystem::GetComp<ControllerComponent>(entity) = *states.at(entity).cont;
	}
}

void SyncState::applyState() {
    for(auto&& state : states) {
        *EntitySystem::GetComp<NetworkDataComponent>(state.first) = state.second.data;
        if(state.second.cont.has_value()) {
            *EntitySystem::GetComp<ControllerComponent>(state.first) = *state.second.cont;
        }
    }
}

bool SyncState::State::operator==(const SyncState::State& other) const {
    return data == other.data && cont == other.cont;
}

bool SyncState::State::operator!=(const SyncState::State& other) const {
    return !((*this) == other);
}
