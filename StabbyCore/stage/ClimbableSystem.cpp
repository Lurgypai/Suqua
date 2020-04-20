#include "ClimbableSystem.h"
#include "ClimbableComponent.h"
#include "PositionComponent.h"

void ClimbableSystem::updateClimbables() {
	if (EntitySystem::Contains<ClimbableComponent>()) {
		for (auto& climbable : EntitySystem::GetPool<ClimbableComponent>()) {
			climbable.collider.pos = EntitySystem::GetComp<PositionComponent>(climbable.getId())->pos;
		}
	}
}
