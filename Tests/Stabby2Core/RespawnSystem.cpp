#include "RespawnSystem.h"
#include "PlayerComponent.h"

using PlayerState = PlayerComponent::PlayerState;

void RespawnSystem::update() {
	if (EntitySystem::Contains<PlayerComponent>()) {
		for (auto& pc : EntitySystem::GetPool<PlayerComponent>()) {
			if (pc.getState() == PlayerState::respawning) {
				pc.doRespawn();
			}
		}
	}
}