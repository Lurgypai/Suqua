#pragma once
#include "Vec2.h"
#include "Connection.h"
#include "PositionComponent.h"
#include "ControllerComponent.h"

#include "player.h"
#include "network.h"

#include "../player/ServerPlayerComponent.h"

class User {
public:
	User(PlayerManager* players, WeaponManager* weapons,  NetworkId id_, ConnectionPtr&& con);

	void deleteUser();

	EntityId getId() const;
	PeerId getPeerId() const;
	Connection * getConnection();
	PlayerLC & getPlayer();
	ServerPlayerComponent& getServerPlayer();
	PhysicsComponent & getPhysics();
	PositionComponent & getPosition();
	PlayerStateComponent & getPlayerState();
	CombatComponent & getCombat();
	OnlineComponent& getOnline();
	ControllerComponent& getController();
	NameTagComponent& getNameTag();

private:
	EntityId id;
	PeerId peerId;
	ConnectionPtr con;
};