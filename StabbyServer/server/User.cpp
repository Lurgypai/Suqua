
#include "User.h"
#include "EntityBaseComponent.h"

User::User(PlayerManager* players, WeaponManager* weapons, NetworkId id_, ConnectionPtr && con_) :
	peerId{id_},
	con{std::move(con_)}
{
	id = players->makePlayer(*weapons);
	EntitySystem::MakeComps<ServerPlayerComponent>(1, &id);
}

void User::deleteUser() {
	EntityBaseComponent* entity = EntitySystem::GetComp<EntityBaseComponent>(id);
	entity->isDead = true;
}

EntityId User::getId() const {
	return id;
}

PeerId User::getPeerId() const {
	return peerId;
}

Connection * User::getConnection() {
	return con.get();
}

PlayerLC & User::getPlayer() {
	return *EntitySystem::GetComp<PlayerLC>(id);
}

ServerPlayerComponent& User::getServerPlayer() {
	return *EntitySystem::GetComp<ServerPlayerComponent>(id);
}

PhysicsComponent & User::getPhysics() {
	return *EntitySystem::GetComp<PhysicsComponent>(id);
}

PositionComponent & User::getPosition() {
	return *EntitySystem::GetComp<PositionComponent>(id);
}

PlayerStateComponent & User::getPlayerState() {
	return *EntitySystem::GetComp<PlayerStateComponent>(id);
}

CombatComponent& User::getCombat() {
	return *EntitySystem::GetComp<CombatComponent>(id);
}

OnlineComponent& User::getOnline() {
	return *EntitySystem::GetComp<OnlineComponent>(id);
}

ControllerComponent& User::getController() {
	return *EntitySystem::GetComp<ControllerComponent>(id);
}

NameTagComponent& User::getNameTag() {
	return *EntitySystem::GetComp<NameTagComponent>(id);
}
