#include "AimToLStickComponent.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"

AimToLStickComponent::AimToLStickComponent(EntityId id_) :
	id{ id_ }
{
	if (id != 0) {
		if (!EntitySystem::Contains<DirectionComponent>() || !EntitySystem::GetComp<DirectionComponent>(id)) {
			EntitySystem::MakeComps<DirectionComponent>(1, &id);
		}
		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}
	}
}

void AimToLStickComponent::update() {
	auto* dirComp = EntitySystem::GetComp<DirectionComponent>(id);
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);

	dirComp->setDir(contComp->getController().stick2.angle());
}