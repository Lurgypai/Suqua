#include "PlayerSFX.h"
#include "SoundComponent.h"
#include "../player/PlayerLC.h"
#include "../graphics/PlayerGC.h"
#include <iostream>

PlayerSFX::PlayerSFX(EntityId id_) :
	id{id_},
	prevGrounded{false}
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
	if (EntitySystem::Contains<PlayerLC>()) {

		SoundComponent* sounds = EntitySystem::GetComp<SoundComponent>(id);

		PlayerLC* player = EntitySystem::GetComp<PlayerLC>(id);
		const PlayerState& state = player->getState();
		auto currHealth = state.health;
		if (currHealth < prevHealth) {
			sounds->triggerSound("player.hurt1", state.pos);
			if (currHealth <= 0) {
				sounds->triggerSound("player.death1", state.pos);
			}
		}
		prevHealth = currHealth;

		if (state.state == State::rolling && ! prevRolling) {
			sounds->triggerSound("player.swing1", state.pos);
		}
		prevRolling = state.state == State::rolling;

		if (state.activeAttack && state.activeAttack != prevAttack) {
			sounds->triggerSound("player.swing1", state.pos);
		}
		prevAttack = state.activeAttack;

		PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

		RenderComponent* gfx = EntitySystem::GetComp<RenderComponent>(id);
		auto animSprite = gfx->getDrawable<AnimatedSprite>();
		if (physics->grounded) {
			if (!prevGrounded) {
				sounds->triggerSound("player.land1", state.pos);
			}
			if (animSprite->getFrame() == 3 || animSprite->getFrame() == 11) {
				sounds->triggerSound("player.step1", state.pos);
			}
		}
		else {
			if (prevGrounded && state.vel.y < 0) {
				sounds->triggerSound("player.jump1", state.pos);
			}
			if (animSprite->getFrame() == 52 && prevFrame != 52) {
				sounds->triggerSound("player.step1", state.pos);
			}
			else if (animSprite->getFrame() == 53 && prevFrame != 53) {
				sounds->triggerSound("player.step1", state.pos);
			}
			prevFrame = animSprite->getFrame();
		}
		prevGrounded = physics->grounded;
	}
}
