#include "ClientPlayerComponent.h"
#include "ControllerComponent.h"
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
	states.emplace_back(PlrContState{ playerState, controller.getState(), controller.getPrevState() });

	DebugFIO::Out("c_out.txt") << "stored state at time " << clientTime << ", pos: " << playerState.pos << ", vel: " << playerState.vel << '\n';

	if (EntitySystem::GetComp<OnlineComponent>(id)) {
		OnlineComponent& online = *EntitySystem::GetComp<OnlineComponent>(id);
		DebugFIO::Out("plr_log.txt") << clientTime << '\t';
		DebugFIO::Out("plr_log.txt") << online.getNetId() << '\t';
		DebugFIO::Out("plr_log.txt") << static_cast<int>(controller.getState()) << '\t';
		DebugFIO::Out("plr_log.txt") << static_cast<int>(controller.getPrevState()) << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.pos << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.vel << '\t';
		DebugFIO::Out("plr_log.txt") << static_cast<int>(playerState.state) << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.rollFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.activeAttack << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.attackFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.health << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.stunFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.facing << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.spawnPoint << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.attackFreezeFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.frozen << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.attackSpeed << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.moveSpeed << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.healFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.healDelay << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.teamId << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.stamina << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.staminaRechargeFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.deathFrame << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.weaponTag << '\t';
		DebugFIO::Out("plr_log.txt") << playerState.userTag << '\t';
		DebugFIO::Out("plr_log.txt") << '\n';
	}
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
