#include "pch.h"
#include "User.h"
#include "PhysicsAABBLC.h"
#include "PlayerData.h"

User::User(NetworkId id_, ConnectionPtr && con_) :
	netId{id_},
	con{std::move(con_)}
{
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<ServerPlayerLC>(1, &id);
	EntitySystem::MakeComps<PhysicsAABBLC>(1, &id);

	EntitySystem::GetComp<ServerPlayerLC>(id)->setPos({ -PLAYER_WIDTH / 2, -PLAYER_HEIGHT });
	EntitySystem::GetComp<PhysicsAABBLC>(id)->setRes({PLAYER_WIDTH, PLAYER_HEIGHT});
}

EntityId User::getId() const {
	return id;
}

NetworkId User::getNetId() const {
	return netId;
}

Connection * User::getConnection() {
	return con.get();
}

ServerPlayerLC & User::getPlayer() {
	return *EntitySystem::GetComp<ServerPlayerLC>(id);
}
