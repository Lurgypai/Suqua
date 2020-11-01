#pragma once
#include "EntitySystem.h"
#include "Controller.h"
#include "PhysicsSystem.h"
#include <unordered_map>

#include "network.h"
#include "combat.h"

//contains the last time a ClientPlayer was updated, and their EntityId
struct LastClientTick {
	Time_t clientTime;
	Time_t gameTime;
	EntityId player;
};

class ClientPlayerSystem {
public:
	ClientPlayerSystem(PhysicsSystem* physicsSystem_, CombatSystem* combatSystem_);
	void update(Time_t gameTime, Time_t clientTime);
	void update(Time_t gameTime, Time_t clientTime, const EntityId& player);
	void repredict(EntityId playerId, NetworkId netId, PlayerState state, const DynamicBitset& changedFields, double timeDelta);

	//gets wether or not the most recent playerstate was more recent than the last synch time, and if it was found and applied. resets if the most recent playerstate was too old, or not applied
	bool getClientWasSynchronized() const;
	//same as synchronized, only resets if it was not applied
	bool getClientBehindServer() const;
private:
	PhysicsSystem* physicsSystem;
	CombatSystem* combatSystem;
	bool clientWasSynchronized;
	bool clientBehindServer;
	std::unordered_map<NetworkId, LastClientTick> players;
};