#include "PlayerSFX.h"
#include "SoundComponent.h"
#include "../player/PlayerLC.h"

PlayerSFX::PlayerSFX(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<SoundComponent>() || EntitySystem::GetComp<SoundComponent>(id) == nullptr) {
			EntitySystem::MakeComps<SoundComponent>(1, &id);
		}
	}
}

EntityId PlayerSFX::getId() const {
	return id;
}

void PlayerSFX::update() {
	if (EntitySystem::Contains<PlayerStateComponent>()) {
		PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
		const PlayerState& state = player->getState();
		auto currHealth = state.health;
		if (currHealth < prevHealth) {
			SoundComponent* sounds = EntitySystem::GetComp<SoundComponent>(id);
			sounds->triggerSound("player.hurt1", state.pos);
		}

		prevHealth = currHealth;
	}
}
