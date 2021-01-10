#include "PlayerSFX.h"
#include "SoundComponent.h"

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
		PlayerStateComponent* state = EntitySystem::GetComp<PlayerStateComponent>(id);
		auto currState = state->playerState.state;
		if (currState == State::stunned && currState != prevState) {
			SoundComponent* sounds = EntitySystem::GetComp<SoundComponent>(id);
			//sounds->triggerSound("player.hurt1");
		}

		prevState = currState;
	}
}
