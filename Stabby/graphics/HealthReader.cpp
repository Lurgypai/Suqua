#include "HealthReader.h"
#include "player.h"
#include "combat.h"

HealthReader::HealthReader(EntityId target_) :
	StatReader{target_}
{}

HealthReader::~HealthReader()
{}

float HealthReader::getRatio() {
	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(target);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(target);
	return static_cast<float>(player->getState().health) / combat->stats.maxHealth;
}

StatReader* HealthReader::clone() {
	return new HealthReader(*this);
}
