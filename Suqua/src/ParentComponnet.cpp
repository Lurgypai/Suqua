#include "ParentComponent.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"
#include "EntityBaseComponent.h"

ParentComponent::ParentComponent(EntityId id_,
        OffsetMode offsetMode_,
        EntityId parentId_,
        const Vec2f& baseOffset_,
        const Vec2f& effectedOffset_) :
	id{ id_ },
	offsetMode{offsetMode_},
	parentId{ parentId_ },
	baseOffset{ baseOffset_ },
	effectedOffset{effectedOffset_}
{
    if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
        EntitySystem::MakeComps<PositionComponent>(1, &id);
    }
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
    default:
    break;
	}
}
