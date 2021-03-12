#include "PlayerSFX.h"
#include "SoundComponent.h"
#include "../player/PlayerLC.h"
#include "../graphics/PlayerGC.h"
#include "NetworkDataComponent.h"
#include "player.h"
#include "combat.h"
#include "PositionData.h"
#include <iostream>

using NDC = NetworkDataComponent;

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
		NDC* data = EntitySystem::GetComp<NDC>(id);
		Vec2f pos = {data->get<float>(X), data->get<float>(Y)};
		PlayerLC::State state = static_cast<PlayerLC::State>(data->get<char>(STATE));
		auto currHealth = data->get<int32_t>(HEALTH);
		if (currHealth < prevHealth) {
			sounds->triggerSound("player.hurt1", pos);
			if (currHealth <= 0) {
				sounds->triggerSound("player.death1", pos);
			}
		}
		prevHealth = currHealth;

		if (state == PlayerLC::State::rolling && ! prevRolling) {
			sounds->triggerSound("player.roll2", pos);
		}
		prevRolling = state == PlayerLC::State::rolling;

		if (data->get<uint32_t>(ACTIVE_ATTACK) && data->get<uint32_t>(ACTIVE_ATTACK) != prevAttack) {
			sounds->triggerSound("player.swing2", pos);
		}
		prevAttack = data->get<uint32_t>(ACTIVE_ATTACK);

		PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

		RenderComponent* gfx = EntitySystem::GetComp<RenderComponent>(id);
		auto animSprite = gfx->getDrawable<AnimatedSprite>();
		if (data->get<bool>(GROUNDED)) {
			if (!prevGrounded) {
				sounds->triggerSound("player.land1", pos);
			}
			if (!data->get<uint32_t>(ACTIVE_ATTACK) && ((animSprite->getFrame() == 3 && prevFrame != 3) || (animSprite->getFrame() == 11 && prevFrame != 11))) {
				sounds->triggerSound("player.step1", pos);
			}
			if (animSprite->getFrame() == 33 && prevFrame != 33) {
				sounds->triggerSound("player.land1", pos);
			}
		}
		else {
			if (prevGrounded && data->get<float>(YVEL) < 0) {
				sounds->triggerSound("player.jump1", pos);
			}
			if (animSprite->getFrame() == 52 && prevFrame != 52) {
				sounds->triggerSound("player.step1", pos);
			}
			else if (animSprite->getFrame() == 53 && prevFrame != 53) {
				sounds->triggerSound("player.step1", pos);
			}
		}
		prevGrounded = data->get<bool>(GROUNDED);

		prevFrame = animSprite->getFrame();
	}
}
