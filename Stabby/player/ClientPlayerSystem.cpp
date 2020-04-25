
#include "ClientPlayerSystem.h"
#include "ClientPlayerComponent.h"
#include "DebugFIO.h"
#include <iostream>
#include <ControllerComponent.h>

ClientPlayerSystem::ClientPlayerSystem(PhysicsSystem* physicsSystem_, CombatSystem* combatSystem_) :
	physicsSystem{ physicsSystem_ },
	combatSystem{combatSystem_},
	players{}
{}

void ClientPlayerSystem::update(Time_t gameTime, Time_t clientTime) {
	if (EntitySystem::Contains<ClientPlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<ClientPlayerComponent>()) {
			ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(player.getid());
			player.storePlayerState(gameTime, clientTime, controller->getController());
		}
	}
}

void ClientPlayerSystem::update(Time_t gameTime, Time_t clientTime, const EntityId& player_) {
	ClientPlayerComponent* player = EntitySystem::GetComp<ClientPlayerComponent>(player_);
	ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(player->getid());
	player->storePlayerState(gameTime, clientTime, controller->getController());
}

void ClientPlayerSystem::repredict(EntityId playerId, NetworkId netId, const PlayerState& state, double timeDelta) {
	ClientPlayerComponent* clientPlayer = EntitySystem::GetComp<ClientPlayerComponent>(playerId);
	auto player = players.find(netId);
	if (player == players.end()) {
		players[netId] = LastClientTick{clientPlayer->getEarliestTime(), 0, playerId};
	}
	
	LastClientTick& lastTick = players[netId];
	if (state.clientTime > lastTick.clientTime) {
		PlrContState plrContState{};

		std::ostream& out = DebugFIO::Out("c_out.txt");
		bool wasUpdated = false;
		while (clientPlayer->pollState(plrContState)) {
			auto& plrState = plrContState.plrState;
			auto& contState = plrContState.contState;
			if (plrState.clientTime == state.clientTime) {
				wasUpdated = true;

				lastTick.gameTime = state.gameTime;
				plrState.gameTime = state.gameTime;

				if (plrState != state) {
					DebugFIO::Out("c_out.txt") << "Prediction failed, resetting at time: " << state.clientTime << '\n';
					if (state.state != plrState.state)
						out << "state: " << static_cast<int>(plrState.state) << ", " << static_cast<int>(state.state) << '\n';
					if (state.rollFrame != plrState.rollFrame)
						out << "rollFrame: " << plrState.rollFrame << ", " << state.rollFrame << '\n';
					if (state.activeAttack != plrState.activeAttack)
						out << "activeAttack: " << plrState.activeAttack << ", " << state.activeAttack << '\n';
					if (state.attackFrame != plrState.attackFrame)
						out << "attackFrame: " << plrState.attackFrame << ", " << state.attackFrame << '\n';
					if (state.health != plrState.health)
						out << "health: " << plrState.health << ", " << state.health << '\n';
					if (state.stunFrame != plrState.stunFrame)
						out << "stunFrame: " << plrState.stunFrame << ", " << state.stunFrame << '\n';
					if (state.facing != plrState.facing)
						out << "facing: " << plrState.facing << ", " << state.facing << '\n';
					if (state.spawnPoint != plrState.spawnPoint)
						out << "spawnPoint: " << plrState.spawnPoint << ", " << state.spawnPoint << '\n';
					if (state.attackFreezeFrame != plrState.attackFreezeFrame)
						out << "attackFreezeFrame: " << plrState.attackFreezeFrame << ", " << state.attackFreezeFrame << '\n';
					if (state.healFrame != plrState.healFrame)
						out << "healFrame" << plrState.healFrame << ", " << state.healFrame << '\n';
					if (state.pos != plrState.pos)
						out << "pos: " << plrState.pos << ", " << state.pos << '\n';
					if (state.vel != plrState.vel)
						out << "vel: " << plrState.vel << ", " << state.vel << '\n';
					if (state.frozen != plrState.frozen)
						out << "frozen: " << plrState.frozen << ", " << state.frozen << '\n';
					if (state.stamina != plrState.stamina)
						out << "stamina: " << plrState.stamina << ", " << state.stamina << '\n';
					if (state.staminaRechargeFrame != plrState.staminaRechargeFrame)
						out << "staminaRechargeFrame: " << plrState.staminaRechargeFrame << ", " << state.staminaRechargeFrame << '\n';

					auto id = lastTick.player;
					PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
					player->setState(state);

					PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);

					Controller currentController = Controller{ contState };

					//now reevaulate. Remove all stored states, and replace.
					auto states = clientPlayer->readAllStates();
					//store the current state
					clientPlayer->storePlayerState(plrState.gameTime, plrState.clientTime, currentController);

					ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
					for(auto& unprocessedState : states) {
						Controller cont{ unprocessedState.contState };
						controller->getController() = cont;
						player->update(timeDelta);
						physicsSystem->runPhysics(timeDelta, id);
						DebugFIO::Out("c_out.txt") << "Freeze frame before: " << player->getState().attackFreezeFrame << '\n';
						combatSystem->runAttackCheck(timeDelta, id);
						DebugFIO::Out("c_out.txt") << "Freeze frame after: " << player->getState().attackFreezeFrame << '\n';

						clientPlayer->storePlayerState(unprocessedState.plrState.gameTime, unprocessedState.plrState.clientTime, cont);
					}
				}
				lastTick.clientTime = state.clientTime;
				DebugFIO::Out("c_out.txt") << "Up to date at time " << lastTick.clientTime << ".\n";
				break;
			}
			else if (plrState.clientTime > state.clientTime) {
				DebugFIO::Out("c_out.txt") << "The server is too far behind the stored controller states. Waiting for a packet thats up to date...\n";
				break;
			}
		}
		if (!wasUpdated) {
			DebugFIO::Out("c_out.txt") << "Client wasn't resynced. lastTick: " << lastTick.clientTime << ", clientTime from server: " << state.clientTime << ".\n";
		}
	}
	else {
		DebugFIO::Out("c_out.txt") << "New state is too old. State time: " << state.clientTime << ", last update: " << lastTick.clientTime << ".\n";
	}
}
