#include "OnHitComponent.h"
#include "HealthComponent.h"
#include "CharacterGFXComponent.h"

OnHitComponent::OnHitComponent(EntityId id_) :
	id{ id_ },
	c{1.0f, 1.0f, 1.0f, 1.0f},
	prevHealth{ 0 },
	playingEffect{ false },
	tick{ 0 },
	maxEffectTick{ 10 }
{
}

void OnHitComponent::update() {
	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	auto characterGFXComp = EntitySystem::GetComp<CharacterGFXComponent>(id);
	auto currHealth = healthComp->getHealth();
	if (!playingEffect && prevHealth != currHealth && currHealth < prevHealth) {
		playingEffect = true;
		characterGFXComp->setOverlayAmount(1.0f);
		tick = 0;
	}

	if (tick != maxEffectTick) {
		++tick;
		if (tick == maxEffectTick) {
			characterGFXComp->setOverlayAmount(0.0f);
			playingEffect = false;
		}
	}
	prevHealth = currHealth;
}