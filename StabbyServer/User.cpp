#include "pch.h"
#include "User.h"
#include "PhysicsAABB.h"
#include "PlayerData.h"

User::User(NetworkId id_, ConnectionPtr && con_) :
	netId{id_},
	con{std::move(con_)}
{
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<ServerPlayerLC>(1, &id);

	EntitySystem::GetComp<ServerPlayerLC>(id)->setPos({ -2, -20 });
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
