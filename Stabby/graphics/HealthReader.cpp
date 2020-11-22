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
	float health = static_cast<float>(player->getState().health);
	if (health >= 0)
		return health / combat->stats.maxHealth;
	else
		return 0;
}

StatReader* HealthReader::clone() {
	return new HealthReader(*this);
}
