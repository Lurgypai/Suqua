#include "pch.h"
#include "ClientPlayerLC.h"
#include "ServerClientData.h"
#include <algorithm>
#include <iostream>

ClientPlayerLC::ClientPlayerLC(EntityId id_) :
	PlayerLC{id_},
	last{0}
{}

void ClientPlayerLC::update(Time_t now, double timeDelta, const Controller & controller) {
	PlayerLC::update(timeDelta, controller);

	//maintain constant max size
	if (states.size() >= BUFFER_SIZE)
		states.pop_front();

	states.emplace_back(TotalPlayerState{ PlayerState{state, now, pos, vel, rollFrame, attack.getActiveId(), attack.getCurrFrame(), health, stunFrame}, controller.getState() });
}

void ClientPlayerLC::repredict(const PlayerState & state) {
	if (state.when > last) {
		for (auto i = 0; i != states.size(); ++i) {
			auto tstate = states[i];
			if (tstate.plr.when == state.when) {
				//reevaulate current pos
				states.erase(states.begin(), states.begin() + i + 1);
				if (tstate.plr != state) {
					std::cout << "Prediction failed, re-predicting\n";

					this->state = state.state;
					pos = state.pos;
					vel = state.vel;
					rollFrame = state.rollFrame;
					attack.setActive(state.activeAttack);
					attack.setFrame(state.attackFrame);
					health = state.health;
					stunFrame = state.stunFrames;

					//move our remaining values out, and then clear
					std::deque<TotalPlayerState> toUpdate{ std::move(states) };
					states.clear();

					//now reevaulate, this will refill states
					for (auto& updateState : toUpdate) {
						update(updateState.plr.when, CLIENT_TIME_STEP, Controller{updateState.in});
					}
				}

				last = state.when;
				break;
			}
		}
	}
}

std::string ClientPlayerLC::getHeadPath() {
	return "images/head.png";
}

Vec2f ClientPlayerLC::getCenter() {
	return pos + Vec2f{PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2};
}
