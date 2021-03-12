#include "HealthReader.h"
#include "player.h"
#include "combat.h"
#include "NetworkDataComponent.h"

HealthReader::HealthReader(EntityId target_) :
	StatReader{target_}
{}

HealthReader::~HealthReader()
{}

float HealthReader::getRatio() {
	NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(target);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(target);
	float health = static_cast<float>(data->get<int32_t>(HEALTH));
	if (health >= 0)
		return health / combat->stats.maxHealth;
	else
		return 0;
}

StatReader* HealthReader::clone() {
	return new HealthReader(*this);
}
