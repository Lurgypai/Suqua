#include "ClimbableSystem.h"
#include "ClimbableComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"

void ClimbableSystem::updateClimbables() {
	if (EntitySystem::Contains<ClimbableComponent>()) {
		for (auto& climbable : EntitySystem::GetPool<ClimbableComponent>()) {
			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(climbable.getId());
			climbable.collider.pos = { data->get<float>(X), data->get<float>(Y) };
		}
	}
}
