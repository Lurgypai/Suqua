#include "GunGFXComponent.h"
#include "RenderComponent.h"
#include "DirectionComponent.h"

GunGFXComponent::GunGFXComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
		}

		auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
		renderComp->loadDrawable<Sprite>("gun");
		renderComp->offset = {2.5, 2};
		
		auto sprite = renderComp->getDrawable<Sprite>();
		sprite->setOrigin({ 1, 1});
	}
}

EntityId GunGFXComponent::getId() const {
	return id;
}

void GunGFXComponent::update() {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	auto sprite = renderComp->getDrawable<Sprite>();
	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);
	int dirDeg = directionComp->getDir() * 180 / 3.14159;

	if (dirDeg < -90 || dirDeg > 90) {
		sprite->verticalFlip = true;
	}
	else {
		sprite->verticalFlip = false;
	}

	if (dirDeg >= -135 && dirDeg < -45) {
		sprite->setDepth(1.0);
	}
	else {
		sprite->setDepth(-1.0);
	}

	sprite->setAngle(directionComp->getDir() * 180 / 3.14159);
}
