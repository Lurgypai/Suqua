#include "HurtboxComponent.h"
#include "PositionComponent.h"

HurtboxComponent::HurtboxComponent(EntityId id_, const Vec2f& offset_, const Vec2f& res) :
	id{ id_ },
	hurtbox{{}, res},
	offset{offset_ }
{}

void HurtboxComponent::update() {
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	hurtbox.pos = posComp->pos + offset;
}
