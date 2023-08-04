#include "ParentComponent.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"
#include "EntityBaseComponent.h"

ParentComponent::ParentComponent(EntityId id_) :
	id{ id_ },
	parentId{ 0 },
	baseOffset{},
	effectedOffset{},
	offsetMode{OffsetMode::none}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

EntityId ParentComponent::getId() const {
	return id;
}

void ParentComponent::update() {
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	if (!baseComp->isActive) return;

	auto parentPosComp = EntitySystem::GetComp<PositionComponent>(parentId);
	if (!parentPosComp) return;

	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	auto basePos = parentPosComp->getPos() + baseOffset;
	switch (offsetMode) {
	case OffsetMode::none:
	{
		posComp->setPos(basePos);
	
	}
	break;
	case OffsetMode::cardinal_left_right:
	{
		auto dirComp = EntitySystem::GetComp<DirectionComponent>(parentId);
		float x = std::cos(dirComp->getDir());
		float dir = x < 0 ? -1.0f : 1.0f;

		auto pos = basePos + Vec2f{ effectedOffset.x * dir, effectedOffset.y };
		posComp->setPos(pos);
	}
	break;
	}
}