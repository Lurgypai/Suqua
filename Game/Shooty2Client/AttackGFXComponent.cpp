#include "AttackGFXComponent.h"
#include "HealthComponent.h"
#include "CharacterGFXComponent.h"
#include "ControllerComponent.h"

AttackGFXComponent::AttackGFXComponent(EntityId id_) :
	id{ id_ },
	prevButtonDown{ false },
	playingEffect{ false },
	tick{ 0 },
	maxEffectTick{ 80 }
{
}

void AttackGFXComponent::update() {
    auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	auto characterGFXComp = EntitySystem::GetComp<CharacterGFXComponent>(id);
    bool currButtonDown = controllerComp->getController()[ControllerBits::BUTTON_11];

	if (!playingEffect && (prevButtonDown != currButtonDown) && currButtonDown) {
		playingEffect = true;
        characterGFXComp->playAnimation("attack", false);
		tick = 0;
	}

	if (tick != maxEffectTick) {
		++tick;
		if (tick == maxEffectTick) {
            characterGFXComp->stopAnimation();
			playingEffect = false;
		}
	}
    prevButtonDown = currButtonDown;
}
