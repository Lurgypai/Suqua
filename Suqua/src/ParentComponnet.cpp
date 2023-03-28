#include "ParentComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"

ParentComponent::ParentComponent(EntityId id_) :
	id{ id_ },
	parentId{ 0 }
{
	if (id != 0) {
		if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
			EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
		}

		auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
		ndc->set<float>(X, 0.0f);
		ndc->set<float>(Y, 0.0f);
	}
}

EntityId ParentComponent::getId() const {
	return id;
}

void ParentComponent::update() {
	auto parentNDC = EntitySystem::GetComp<NetworkDataComponent>(parentId);
	if (!parentNDC) return;

	float x = parentNDC->get<float>(X);
	float y = parentNDC->get<float>(Y);

	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	ndc->get<float>(X) = x + offset.x;
	ndc->get<float>(Y) = y + offset.y;
}