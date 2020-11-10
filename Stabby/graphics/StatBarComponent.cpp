#include "StatBarComponent.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "PositionComponent.h"

StatBarComponent::StatBarComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
			EntitySystem::GetComp<RenderComponent>(id)->loadDrawable<RectDrawable>();
			EntitySystem::GetComp<RenderComponent>(id)->getDrawable<RectDrawable>()->filled = true;
			EntitySystem::GetComp<RenderComponent>(id)->getDrawable<RectDrawable>()->c = {1.0, 1.0, 1.0, 1.0};
			EntitySystem::GetComp<RenderComponent>(id)->getDrawable<RectDrawable>()->depth = -1.0;
		}
	}
}

StatBarComponent::StatBarComponent(const StatBarComponent& other) :
	id{ other.id },
	statReader{}
{
	if (other.statReader) {
		statReader = StatReaderPtr{ other.statReader->clone() };
	}
	else {
		statReader = nullptr;
	}
}

StatBarComponent& StatBarComponent::operator=(const StatBarComponent& other) {
	id = other.id;
	if (other.statReader) {
		statReader = StatReaderPtr{ other.statReader->clone() };
	}
	else {
		statReader = nullptr;
	}
	return *this;
}

EntityId StatBarComponent::getId() const {
	return id;
}

void StatBarComponent::update() {
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	RectDrawable* rect = render->getDrawable<RectDrawable>();
	rect->shape = getCurrBarSize();

	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
	pos->pos = rect->shape.pos;
}

void StatBarComponent::setTarget(EntityId target_) {
	target = target_;
}

AABB StatBarComponent::getCurrBarSize() const {
	return AABB{ fullSize.pos, {fullSize.res.x * statReader->getRatio(), fullSize.res.y} };
}
