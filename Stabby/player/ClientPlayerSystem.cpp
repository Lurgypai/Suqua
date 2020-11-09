
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

void ClientPlayerSystem::repredict(EntityId playerId, NetworkId netId, PlayerState state, const DynamicBitset& changedFields, double timeDelta) {
	ClientPlayerComponent* clientPlayer = EntitySystem::GetComp<ClientPlayerComponent>(playerId);
	auto player = players.find(netId);
	if (player == players.end()) {
		players[netId] = LastClientTick{clientPlayer->getEarliestTime(), 0, playerId};
	}
	
	LastClientTick& lastTick = players[netId];
	clientWasSynchronized = false;
	if (state.clientTime > lastTick.clientTime) {
		PlrContState plrContState{};

		std::ostream& out = DebugFIO::Out("c_out.txt");
		clientBehindServer = true;

		while (clientPlayer->pollState(plrContState)) {
			auto& plrState = plrContState.plrState;
			auto& contState = plrContState.contState;
			auto& prevContState = plrContState.prevContState;
			if (plrState.clientTime == state.clientTime) {
				clientWasSynchronized = true;
				clientBehindServer = false;

				lastTick.gameTime = state.gameTime;
				plrState.gameTime = state.gameTime;

				//compare to bitset, and set all values that weren't changed
				using Bits = PlayerState::StateBits;
				if (!changedFields[Bits::b_state])
					state.state = plrState.state;
				if (!changedFields[Bits::b_roll_frame])
					state.rollFrame = plrState.rollFrame;
				if (!changedFields[Bits::b_active_attack])
					state.activeAttack = plrState.activeAttack;
				if (!changedFields[Bits::b_attack_frame])
					state.attackFrame = plrState.attackFrame;
				if (!changedFields[Bits::b_health])
					state.health = plrState.health;
				if (!changedFields[Bits::b_stun_frame])
					state.stunFrame = plrState.stunFrame;
				if (!changedFields[Bits::b_heal_frame])
					state.healFrame = plrState.healFrame;
				if (!changedFields[Bits::b_heal_delay])
					state.healDelay = plrState.healDelay;
				if (!changedFields[Bits::b_facing])
					state.facing = plrState.facing;
				if (!changedFields[Bits::b_spawn_point])
					state.spawnPoint = plrState.spawnPoint;
				if (!changedFields[Bits::b_attack_freeze_frame])
					state.attackFreezeFrame = plrState.attackFreezeFrame;
				if (!changedFields[Bits::b_team_id])
					state.teamId = plrState.teamId;
				if (!changedFields[Bits::b_stamina])
					state.stamina = plrState.stamina;
				if (!changedFields[Bits::b_stamina_recharge_frame])
					state.staminaRechargeFrame = plrState.staminaRechargeFrame;
				if (!changedFields[Bits::b_death_frame])
					state.deathFrame = plrState.deathFrame;
				if (!changedFields[Bits::b_attack_speed])
					state.attackSpeed = plrState.attackSpeed;
				if (!changedFields[Bits::b_move_speed])
					state.moveSpeed = plrState.moveSpeed;
				if (!changedFields[Bits::b_weapon_tag])
					state.weaponTag = plrState.weaponTag;
				if (!changedFields[Bits::b_pos])
					state.pos = plrState.pos;
				if (!changedFields[Bits::b_vel])
					state.vel = plrState.vel;
				if (!changedFields[Bits::b_frozen])
					state.frozen = plrState.frozen;
				if (!changedFields[Bits::b_user_tag])
					state.userTag = plrState.userTag;
				if (!changedFields[Bits::b_invulnerable])
					state.invulnerable = plrState.invulnerable;

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
					if(state.weaponTag != plrState.weaponTag)
						out << "weaponTag: " << plrState.weaponTag << ", " << state.weaponTag << '\n';
					if (state.invulnerable != plrState.invulnerable)
						out << "invulnerable: " << plrState.invulnerable << ", " << state.invulnerable << '\n';

					auto id = lastTick.player;
					PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
					player->setState(state);
					//set weapon

					PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);

					Controller currentController = Controller{ contState, prevContState };

					//now reevaulate. Remove all stored states, and replace.
					auto states = clientPlayer->readAllStates();
					//store the current state
					clientPlayer->storePlayerState(plrState.gameTime, plrState.clientTime, currentController);

					ControllerComponent* controller = EntitySystem::GetComp<ControllerComponent>(id);
					for(auto& unprocessedState : states) {
						physicsSystem->runPhysics(timeDelta, id);
						combatSystem->runAttackCheck(timeDelta, id);

						Controller cont{ unprocessedState.contState, unprocessedState.prevContState };
						controller->getController() = cont;
						player->update(timeDelta);

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
		if (clientBehindServer) {
			DebugFIO::Out("c_out.txt") << "Client wasn't resynced, behind the server. lastTick: " << lastTick.clientTime << ", clientTime from server: " << state.clientTime << ".\n";
		}
	}
	else {
		DebugFIO::Out("c_out.txt") << "New state is too old. State time: " << state.clientTime << ", last update: " << lastTick.clientTime << ".\n";
	}
}

bool ClientPlayerSystem::getClientWasSynchronized() const {
	return clientWasSynchronized;
}

bool ClientPlayerSystem::getClientBehindServer() const {
	return clientBehindServer;
}

