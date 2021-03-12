#include "StaminaReader.h"
#include "player.h"
#include "combat.h"
#include "NetworkDataComponent.h"

StaminaReader::~StaminaReader()
{}

StaminaReader::StaminaReader(EntityId target_) :
	StatReader{ target_ }
{}

float StaminaReader::getRatio() {
	NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(target);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(target);
	float stamina = static_cast<float>(data->get<uint32_t>(STAMINA));
	if (stamina >= 0)
		return stamina / combat->staminaMax;
	else
		return 0;
}

StatReader* StaminaReader::clone() {
	return new StaminaReader(*this);
}
