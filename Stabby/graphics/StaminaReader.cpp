#include "StaminaReader.h"
#include "player.h"
#include "combat.h"

StaminaReader::~StaminaReader()
{}

StaminaReader::StaminaReader(EntityId target_) :
	StatReader{ target_ }
{}

float StaminaReader::getRatio() {
	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(target);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(target);
	float stamina = static_cast<float>(player->getState().stamina);
	if (stamina >= 0)
		return stamina / combat->staminaMax;
	else
		return 0;
}

StatReader* StaminaReader::clone() {
	return new StaminaReader(*this);
}
