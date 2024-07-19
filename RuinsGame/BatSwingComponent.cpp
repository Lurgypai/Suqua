#include "BatSwingComponent.h"
#include "HitboxComponent.h"
#include "ParentComponent.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "ControllerComponent.h"
#include "DirectionComponent.h"
#include "SideScrollGFXComponent.h"

BatSwingComponent::BatSwingComponent(EntityId id_) :
	id{id_},
	hitboxId{ 0 },
	delay{ 0 },
	duration{ 0 },
	lag{ 0 },
	tick{ 0 },
	isActive_{false}
{
	if (id != 0) {
		EntitySystem::GenEntities(1, &hitboxId);
		EntitySystem::MakeComps<HitboxComponent>(1, &hitboxId);
		EntitySystem::MakeComps<ParentComponent>(1, &hitboxId);

		auto hitboxComp = EntitySystem::GetComp<HitboxComponent>(hitboxId);
		hitboxComp->collisionsMax = 1;
		hitboxComp->reHitDelay = -1;
		hitboxComp->multipleHits = false;

		hitboxComp->hitbox = { {0, 0}, {15, 7} };
		hitboxComp->deactivate();
		hitboxComp->offset = { 0, 0 };

		auto parentComp = EntitySystem::GetComp<ParentComponent>(hitboxId);
		parentComp->parentId = id;
		parentComp->baseOffset = { -3.5, 3 };
		parentComp->effectedOffset = { -10.5, 0 };
		parentComp->offsetMode = ParentComponent::OffsetMode::cardinal_left_right;

		EntitySystem::GetComp<EntityBaseComponent>(hitboxId)->isActive = false;
	}
}

void BatSwingComponent::update()
{
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	if (!baseComp->isActive) return;

	auto healthComp = EntitySystem::GetComp<HealthComponent>(id);
	if (healthComp->getHealth() <= 0) return;

	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	const auto& cont = controllerComp->getController();
	auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	auto gfxComponent = EntitySystem::GetComp<SideScrollGFXComponent>(id);

	if (!isActive_) {
		if (cont.toggled(ControllerBits::BUTTON_11) && cont[ControllerBits::BUTTON_11]) {
			isActive_ = true;
			tick = 0;
			gfxComponent->playAnimation("bat_swing_1", false);
			dirComp->setLocked(true);
		}
	}

	if (isActive_) {
		if (tick == delay) {
			auto hitboxComp = EntitySystem::GetComp<HitboxComponent>(hitboxId);
			hitboxComp->activate();
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

