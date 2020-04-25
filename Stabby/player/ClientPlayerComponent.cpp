#include "ClientPlayerComponent.h"
#include "DebugFIO.h"

ClientPlayerComponent::ClientPlayerComponent(EntityId id_) :
	id{id_},
	states{}
{}

const EntityId ClientPlayerComponent::getid() const {
	return id;
}

void ClientPlayerComponent::storePlayerState(Time_t gameTime, Time_t clientTime, Controller& controller) {
	PlayerState playerState = EntitySystem::GetComp<PlayerLC>(id)->getState();
	playerState.gameTime = gameTime;
	playerState.clientTime = clientTime;
	states.emplace_back(PlrContState{ playerState, controller.getState() });

	//player state isn't set by physics, only by the update method in playerlc. thus, the effects of physics are not set. need to add a "getState" funtcion to the player, which sets all of the player state stuff and sends it.
	DebugFIO::Out("c_out.txt") << "stored state at time " << clientTime << ", pos: " << playerState.pos << ", vel: " << playerState.vel << '\n';
	DebugFIO::Out("c_out.txt") << "attackFreezeFrame was: " << playerState.attackFreezeFrame << '\n';
}

bool ClientPlayerComponent::pollState(PlrContState& state) {
	if (!states.empty()) {
		state = std::move(states.front());
		DebugFIO::Out("c_out.txt") << "Popped state at time " << state.plrState.clientTime << ".\n";
		states.pop_front();
		return true;
	}
	return false;
}

std::deque<PlrContState> ClientPlayerComponent::readAllStates() {
	DebugFIO::Out("c_out.txt") << "Cleared and returned all states.\n";
	auto retStates = std::move(states);
	states.clear();
	return retStates;
}

Time_t ClientPlayerComponent::getEarliestTime() {
	return states.front().plrState.clientTime;
}
