#include "GunGFXComponent.h"
#include "RenderComponent.h"
#include "ControllerComponent.h"
#include "Sprite.h"

GunGFXComponent::GunGFXComponent(EntityId id_) :
	id{id_},
    sprIndex{}
{
    if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
        EntitySystem::MakeComps<RenderComponent>(1, &id);
    }

    auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
    sprIndex = renderComp->loadDrawable<Sprite>("gun");
    
    Sprite& sprite = renderComp->getDrawable<Sprite>(sprIndex);
    sprite.setOrigin({ 1, 1.5});
    sprite.offset = {2.0, -6};
}

void GunGFXComponent::update() {
	auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	Sprite& sprite = renderComp->getDrawable<Sprite>(sprIndex);
    auto contComp = EntitySystem::GetComp<ControllerComponent>(id);
    float dir = contComp->getController().stick2.angle();
	int dirDeg = dir * 180 / 3.14159;
	if (dirDeg < -90 || dirDeg > 90) {
		sprite.verticalFlip = true;
	}
	else {
		sprite.verticalFlip = false;
	}

	if (dirDeg >= -135 && dirDeg < -45) {
		sprite.setDepth(1.0);
	}
	else {
		sprite.setDepth(-1.0);
	}

	sprite.setAngle(dir * 180 / 3.14159);
}
