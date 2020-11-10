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
	return static_cast<float>(player->getState().stamina) / combat->staminaMax;
}

StatReader* StaminaReader::clone() {
	return new StaminaReader(*this);
}
