#include "DaemonGFXComponent.h"
#include "PositionComponent.h"

DaemonGFXComponent::DaemonGFXComponent(EntityId id_) :
	id{id_},
	emitter{ Vec2f{}, 2, "exit", 0.02f}
{}

void DaemonGFXComponent::update(float timeDelta) {
	auto* posComp = EntitySystem::GetComp<PositionComponent>(id);
	emitter.pos = posComp->pos;
	emitter.update(timeDelta);
}