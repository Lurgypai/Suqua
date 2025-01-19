#include "BasicAttackComponent.h"
#include "ControllerComponent.h"
#include "HitboxComponent.h"
#include "ParentComponent.h"
#include "EntityBaseComponent.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"
#include "CharacterGFXComponent.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "HealthComponent.h"

BasicAttackComponent::BasicAttackComponent(EntityId id_,
        int delay_,
        int duration_,
        int lag_
        ) :
	id{ id_ },
	hitboxId{ 0 },
	delay{ delay_ },
	duration{ duration_ },
	lag{ lag_ },
	tick{ 0 },
	isActive_{false}
{
    /* BIG WARNING
     * This class is not ready to be used. We can't properly nest an entity inside a component being created because there is no way to assign a valid UUID
     */
    EntitySystem::GenEntities(1, &hitboxId);
    // EntitySystem::MakeComps<NetworkDataComponent>(1, &hitboxId,

    EntitySystem::MakeComps<HitboxComponent>(1, &hitboxId,
            Vec2f{-5.f, -10.f},
            Vec2f{10.f, 10.f} );

    EntitySystem::MakeComps<ParentComponent>(1, &hitboxId,
            ParentComponent::OffsetMode::cardinal_left_right,
            id,
            Vec2f{3.f, 4.f},
            Vec2f{8.f, 0.f} );

    EntitySystem::MakeComps<DamageComponent>(1, &hitboxId);

    auto hitboxComp = EntitySystem::GetComp<HitboxComponent>(hitboxId);
    hitboxComp->collisionsMax = 1;
    hitboxComp->reHitDelay = -1;
    hitboxComp->multipleHits = false;
    hitboxComp->deactivate();

    auto damageComp = EntitySystem::GetComp<DamageComponent>(hitboxId);
    damageComp->setDamageCalculator<BasicDamageCalculator>(30);

    EntitySystem::GetComp<EntityBaseComponent>(hitboxId)->isActive = false;
}

void BasicAttackComponent::update()
{
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	if (!baseComp->isActive) return;

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	if (healthComp->getHealth() <= 0) return;

	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	const auto& cont = controllerComp->getController();
	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	auto gfxComponent = EntitySystem::GetComp<CharacterGFXComponent>(id);

	if (!isActive_) {
		if (cont.toggled(ControllerBits::BUTTON_11) && cont[ControllerBits::BUTTON_11]) {
			isActive_ = true;
			tick = 0;
			gfxComponent->playAnimation("attack", false);
		}
	}

	if (isActive_) {
		if (tick == delay) {
			auto hitboxComp = EntitySystem::GetComp<HitboxComponent>(hitboxId);
			hitboxComp->activate();
			dirComp->setLocked(true);
		}
		else if (tick >= delay && tick == delay + duration) {
			auto hitboxComp = EntitySystem::GetComp<HitboxComponent>(hitboxId);
			hitboxComp->deactivate();
		}
		else if (tick >= delay + duration + lag) {
			isActive_ = false;
			dirComp->setLocked(false);
			gfxComponent->stopAnimation();
		}
		++tick;
	}
}

Vec2f BasicAttackComponent::getHitBoxPos() const {
	auto posComp = EntitySystem::GetComp<PositionComponent>(hitboxId);
	return posComp->getPos();
}

//fix the sidedness of the attack
//add animation
//fix ai
